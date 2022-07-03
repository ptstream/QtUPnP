#include "mainwindow.hpp"
#include "helper.hpp"
#include "ui_mainwindow.h"
#include <QProgressBar>
#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>
#include <QElapsedTimer>
#include <QRegularExpression>

USING_UPNP_NAMESPACE

CMainWindow::CMainWindow (QWidget* parent) : QMainWindow (parent), ui (new Ui::CMainWindow)
{
  ui->setupUi (this);

  m_pb = new QProgressBar (this);
  m_pb->setMaximum (10);
  m_pb->setMaximumHeight (16);
  statusBar ()->addPermanentWidget (m_pb);
  connect (&m_timer, &QTimer::timeout, this, &CMainWindow::timeout);
  m_cp = new CControlPoint (this);
  m_cp->setExpandEmbeddedDevices (); // Put a copy of embedded devices in the device map.
  connect (m_cp, SIGNAL(searched(const char*,int,int)), this, SLOT(discoveryLaunched(const char*,int,int)));// Not essential. Can be skipped.
  connect (m_cp, SIGNAL(eventReady(QStringList const &)), this, SLOT(eventReady(QStringList const &)));
  connect (m_cp, SIGNAL(upnpError(int, QString const &)), this, SLOT(upnpError(int, QString const &)));
  connect (m_cp, SIGNAL(networkError(QString const &, QNetworkReply::NetworkError, QString const &)),
           this, SLOT(networkError(QString const &, QNetworkReply::NetworkError, QString const &)));
  connect (m_cp, SIGNAL(newDevice(QString const &)), this, SLOT(newDevice(QString const &)));
  connect (m_cp, SIGNAL(lostDevice(QString const &)), this, SLOT(lostDevice(QString const &)));

  m_ctl.setControlPoint (m_cp);
  ui->m_tabWidget->setCurrentIndex (0);
  m_itemBrush = ui->m_devices->topLevelItem (0)->foreground (0);
  createActionLinks ();
  m_timer.start (500); // Launch discovery when all widgets are polished.
}

CMainWindow::~CMainWindow ()
{
  delete ui;
}

void CMainWindow::closeEvent (QCloseEvent*)
{
  saveDevices ();
  m_cp->close ();
}

void CMainWindow::hideProgressBar (bool hide)
{
  m_pb->setValue (0);
  m_pb->setHidden (hide);
}

QTreeWidgetItem* CMainWindow::findActionItem (QString const & text)
{
  QTreeWidgetItem*        item = nullptr;
  QTreeWidgetItemIterator it (ui->m_services);
  while ((*it) != nullptr)
  {
    QString itemText = (*it)->text(0);
    itemText.remove (QRegularExpression (" \\(\\d+ms\\)"));
    if (itemText == text)
    {
      item = *it;
      break;
    }

    ++it;
  }

  return item;
}

void CMainWindow::createActionLinks ()
{
  m_actionLinks["SetMute"]           = QStringList ({"GetMute"});
  m_actionLinks["SetVolume"]         = QStringList ({"GetVolume"});
  m_actionLinks["SetVolumeDB"]       = QStringList ({"GetVolume", "GetVolumeDB", "GetVolumeDBRange"});
  m_actionLinks["SetAVTransportURI"] = QStringList ({"GetMediaInfo"});
  m_actionLinks["PPS"]               = QStringList ({"GetTransportInfo"});
  m_actionLinks["SetPlayMode"]       = QStringList ({"GetTransportSettings"});
  m_actionLinks["Seek"]              = QStringList ({"GetPositionInfo"});
  m_actionLinks["PN"]                = QStringList ({"GetMediaInfo"});
  m_actionLinks["SelectPreset"]      = QStringList ({"ListPresets"});
}

void CMainWindow::removeActionArgs (QTreeWidgetItem* item)
{
  if (item != nullptr)
  {
    while (item->childCount () != 0)
    {
      QTreeWidgetItem* itemChild = item->takeChild (0);
      delete itemChild;
    }
  }
}

void CMainWindow::applyError (QTreeWidgetItem* item)
{
  if (!CActionManager::lastError ().isEmpty ())
  {
    setItemColor (item, QColor (250, 0, 0));
    statusBar ()->showMessage (CActionManager::lastError ());
  }
}

void CMainWindow::clearError (QTreeWidgetItem* item)
{
  item->setForeground (0, m_itemBrush);
}

// Insert DIDL-Lite elements.
void CMainWindow::insertDidlElems (CItem* item)
{
  QMultiMapIterator<QString, CDidlElem> ite (item->didl ().elems ());
  while (ite.hasNext ())
  {
    ite.next ();
    CDidlElem const & elem  = ite.value (); // DIDL-Lite element.
    QString const   & key   = ite.key (); // Name.
    QString const &   value = elem.value (); // Value.

    QString text = key + ":\"" + value + '\"';
    QTreeWidgetItem* elemItem = new QTreeWidgetItem (item, QStringList (text));

    // Enumerate properties.
    TMProps const & props = elem.props ();
    text                  = key;
    if (!props.isEmpty ())
    {
      QMapIterator<QString, QString> itp (props);
      while (itp.hasNext ())
      {
        itp.next ();
        QString const & name  = itp.key (); // Property name.
        QString const & value = itp.value (); // Property value.
        text                  = name + ":\"" + value + '\"';
        new QTreeWidgetItem (elemItem, QStringList (text));
      }
    }
  }
}

void CMainWindow::updateTree (QTreeWidgetItem* item, CBrowseReply const & reply, EItemType type)
{
  QElapsedTimer ti;
  ti.start ();

  QList<CDidlItem> const & didlItems = reply.items ();
  int                      cItems    = didlItems.size ();
  int                      iItem     = 0;
  for (CDidlItem const & didlItem : didlItems)
  {
    ++iItem;
    CItem* itemChild = new CItem (item, didlItem, type);

    CDidlElem                      didlElem = didlItem.value ("container");
    QMap<QString, QString> const & props    = didlElem.props ();
    if (props.isEmpty ())
    {
      didlElem = didlItem.value ("item");
    }

    QStringList      uris = didlItem.albumArtURIs ();
    QString          thumbnail;
    CDidlItem::EType type = didlItem.type ();
    if (uris.isEmpty () && (type == CDidlItem::ImageItem || type == CDidlItem::Photo))
    {
      uris = didlItem.uris ();
    }

    if (!uris.isEmpty ())
    {
      thumbnail = uris.last ();
    }

    QPixmap pxm;
    if (!thumbnail.isEmpty ())
    {
      pxm = m_pxmCache.search (thumbnail);
      if (pxm.isNull())
      { // Not find in the cache. Get from server.
        float t1 = ti.elapsed () / 1000.0f;
        CDataCaller dc (m_cp->networkAccessManager ());
        QByteArray  pxmBytes = dc.callData (thumbnail);
        float t2 = ti.elapsed () / 1000.0f;
        qDebug () << "Get thumbnail " << "size:" << pxmBytes.size () << ' ' << t2 - t1 << "total time: "<< t2 << 's' << "Item: " << iItem << " of " << cItems;
        if (!pxmBytes.isEmpty ())
        { // Add to the cache.
          pxm = m_pxmCache.add (thumbnail, pxmBytes, QSize (16, 16));
        }
      }
    }

    if (pxm.isNull ())
    { // No pixmap avalaible use generic pixmap.
      char const * pxmName;
      if (!CDidlItem::isContainer (type))
      {
        switch (type)
        {
          case CDidlItem::ImageItem :
          case CDidlItem::Photo :
            pxmName = "image";
            break;

          case CDidlItem::AudioItem :
          case CDidlItem::MusicTrack :
          case CDidlItem::AudioBroadcast :
          case CDidlItem::AudioBook :
          case CDidlItem::AudioProgram :
            pxmName = "audio";
            break;

          case CDidlItem::VideoItem :
          case CDidlItem::Movie :
          case CDidlItem::VideoBroadcast :
          case CDidlItem::MusicVideoClip :
          case CDidlItem::VideoProgram :
            pxmName = "video";
            break;

          default :
            pxmName = "unknown";
            break;
        }
      }
      else
      {
        switch (type)
        {
          case CDidlItem::Container :
            pxmName = "container";
            break;

          case CDidlItem::MusicAlbum :
            pxmName = "music_album";
            break;

          default :
            pxmName = "folder";
            break;
        }
      }

      pxm = QPixmap (QString (":/resources/%1.png").arg (pxmName));
    }

    if (!pxm.isNull ())
    {
      itemChild->setIcon (0, pxm);
    }

    ui->m_services->scrollToItem (itemChild);
    insertDidlElems (itemChild);
  }

  addElapsedTime (item);
}

void CMainWindow::browse (QTreeWidgetItem* item)
{
  CItem*            didlItem = static_cast<CItem*>(item);
  CDidlItem const & didl     = didlItem->didl ();
  QString           id       = didl.isEmpty () ? "0" : didlItem->didl ().value ("container", "id");
  if (!id.isEmpty ())
  {
    removeActionArgs (item);
    item->setExpanded (true);

    CContentDirectory cd (m_cp);
    CBrowseReply      reply = cd.browse (m_deviceUUID, id);
    updateTree (item, reply, BrowseType);
  }
}

void CMainWindow::search (QTreeWidgetItem* item, QString const & criteria)
{
  CItem*            didlItem = static_cast<CItem*>(item);
  CDidlItem const & didl     = didlItem->didl ();
  QString           id       = didl.isEmpty () ? "0" : didlItem->didl ().value ("container", "id");
  if (!id.isEmpty ())
  {
    removeActionArgs (item);
    item->setExpanded (true);
    CContentDirectory cd (m_cp);
    CBrowseReply reply = cd.search (m_deviceUUID, id, criteria);
    updateTree (item, reply, SearchType);
  }
}

QTreeWidgetItem* CMainWindow::insertItem (QTreeWidgetItem* parentItem, CDevice const & device)
{
  QString const &  name     = device.name (); // Get device name. This is the friendly name if it exists, otherwise the name.
  QString const &  uuid     = device.uuid ();
  QString          host     = device.url ().host ();
  quint16          port     = device.url ().port ();
  QString          text     = QString ("%1 [%2:%3] (%4)").arg (name).arg (host).arg (port).arg (uuid);
  QTreeWidgetItem* item     = new QTreeWidgetItem (parentItem, QStringList (text));
  QByteArray       pxmBytes = device.pixmapBytes (m_cp->networkAccessManager ()); // Retreive the pixmap attached at the device.
  if (!pxmBytes.isEmpty ())
  {
    QPixmap pxm;
    pxm.loadFromData (pxmBytes); // CDevice::pixmapBytes return the content of image.
    item->setIcon (0, pxm); // Set item icon.
  }

  item->setData (0, Qt::UserRole, device.uuid ()); // Attach the uuid at the item for future use.
  QList<CDevice> const & subDevices = device.subDevices ();
  for (QList<CDevice>::const_iterator it = subDevices.cbegin (), end = subDevices.end (); it != end; ++it)
  {
    insertItem (item, *it);
  }

  return item;
}

void CMainWindow::saveDevices ()
{
  if (m_storeDevices)
  {
    QList<QPair<QString, QUrl>> devices = m_cp->devicesFinding ();
    QStringList                 dirs    = QStandardPaths::standardLocations (QStandardPaths::TempLocation);
    QFileInfo                   fi (QDir (dirs.first ()), "QtUPnPDevices");
    QFile file (fi.absoluteFilePath ());
    file.open (QIODevice::WriteOnly | QIODevice::Text);
    QTextStream ts (&file);
    for (QPair<QString, QUrl> const & device : devices)
    {
      ts << device.first << ',' << device.second.toString () << '\n';
    }

    file.close ();
  }
}

void CMainWindow::restoreDevices ()
{
  if (m_storeDevices)
  {
    QList<QPair<QString, QUrl>> devices;
    QStringList                 dirs = QStandardPaths::standardLocations (QStandardPaths::TempLocation);
    QFileInfo                   fi (QDir (dirs.first ()), "QtUPnPDevices");
    QFile file (fi.absoluteFilePath ());
    file.open (QIODevice::ReadOnly | QIODevice::Text);
    QTextStream ts (&file);
    for (;;)
    {
      QString     line = ts.readLine ();
      QStringList data = line.split (',');
      if (data.size () == 2)
      {
        devices.append (QPair<QString, QUrl> (data[0], QUrl (data[1])));
      }
      else
      {
        break;
      }
    }

    m_cp->extractDevices (devices);
    file.close ();
  }
}
