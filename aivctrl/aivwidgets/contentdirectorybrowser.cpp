#include "contentdirectorybrowser.hpp"
#include "widgethelper.hpp"
#include "../upnp/pixmapcache.hpp"
#include "../upnp/plugin.hpp"
#include <QScrollBar>
#include <QShortcut>

USING_UPNP_NAMESPACE

int CContentDirectoryBrowserItem::m_albumArtURITimeout = 10000;
QTimer CContentDirectoryBrowser::m_iconUpdateTimer;
int const CContentDirectoryBrowser::m_iconUpdateThresholdTime = 500;
int const CContentDirectoryBrowser::m_iconUpdateRepeatTime = 10;
CContentDirectoryBrowser* CContentDirectoryBrowser::m_directoryBrowser = nullptr;

CContentDirectoryBrowserItem::CContentDirectoryBrowserItem (QListWidget* parent,
      int type) : QListWidgetItem (parent, type)
{
}

CContentDirectoryBrowserItem::CContentDirectoryBrowserItem (const CDidlItem& didlItem, QListWidget* parent,
          int type) : QListWidgetItem (parent, type), m_didlItem (didlItem)
{
  QString const & label = didlItem.title ();
  setText (label);
  updateTooltip ();
}

CContentDirectoryBrowserItem::CContentDirectoryBrowserItem (CContentDirectoryBrowserItem const & other) :
  QListWidgetItem (other), m_didlItem (other.m_didlItem)
{
  updateTooltip ();
  m_iconType = IconNotdefined;
}

QPixmap CContentDirectoryBrowserItem::standardIcon (CDidlItem const & didlItem, QStringList const & defaultPixmaps)
{
  QPixmap          pxm;
  CDidlItem::EType type = didlItem.type ();
  if (type < defaultPixmaps.size ())
  {
    pxm.load (::resIconFullPath (defaultPixmaps[type]));
  }

  return pxm;
}

void CContentDirectoryBrowserItem::updateIcon (CControlPoint* cp, CPixmapCache* cache, int iconIndex,
           QStringList const & defaultPixmaps, QSize const & iconSize)
{
  if (iconType () == CContentDirectoryBrowserItem::IconNotdefined)
  {
    QPixmap pxm;
    QString uri = m_didlItem.albumArtURI (iconIndex);
    if (!uri.isEmpty () && cache != nullptr)
    {
      pxm = cache->search (uri);
    }

    if (pxm.isNull ())
    {
      if (!uri.isEmpty ())
      {
        cp->networkComStarted (CDevice::MediaServer);
      }

      QByteArray pxmBytes = cp->callData (uri, m_albumArtURITimeout);
      if (!uri.isEmpty ())
      {
        cp->networkComEnded (CDevice::MediaServer);
      }

      if (!pxmBytes.isEmpty ())
      {
        if (cache != nullptr)
        {
          cache->add (uri, pxmBytes, iconSize);
          pxm = cache->search (uri);
        }
        else
        {
          pxm.loadFromData (pxmBytes);
        }

        m_iconType = IconServer;
      }
      else
      {
        pxm        = CContentDirectoryBrowserItem::standardIcon (m_didlItem, defaultPixmaps);
        m_iconType = IConStandard;
      }
    }
    else
    {
      m_iconType = IconServer;
    }

    setIcon (QIcon (pxm));
  }
}

void CContentDirectoryBrowserItem::updateTooltip ()
{
  QString string = m_didlItem.title ();
  if (string.isEmpty ())
  {
    string = QObject::tr ("Unknown");
  }

  QString tooltip = TOOLTIPFIELD1 ("Title", string);
  tooltip        += TOOLTIPFIELD ("Album", m_didlItem.album ());
  tooltip        += TOOLTIPFIELD ("Date", m_didlItem.date ());
  tooltip        += TOOLTIPFIELD ("Artist", m_didlItem.artist ());
  tooltip        += TOOLTIPFIELD ("Artist album", m_didlItem.albumArtist ());
  tooltip        += TOOLTIPFIELD ("Composer",  m_didlItem.composer ());
  tooltip        += TOOLTIPFIELD ("Creator", m_didlItem.creator ());
  tooltip        += TOOLTIPFIELD ("Genre", m_didlItem.genre ());
  tooltip        += TOOLTIPFIELD ("Resolution", m_didlItem.resolution ());
  tooltip        += TOOLTIPFIELD ("Size (kB)", m_didlItem.size () / 1024);
  tooltip        += TOOLTIPFIELD ("Bitrate (kbs)", m_didlItem.bitrate () * 8 / 1024);
  tooltip        += TOOLTIPFIELD ("Duration", m_didlItem.duration ());
  tooltip        += TOOLTIPFIELD ("Audio channels", m_didlItem.nrAudioChannels ());
  tooltip        += TOOLTIPFIELD ("Sample frequency (khz)", static_cast<double>(m_didlItem.sampleFrequency ()) / 1000);
  setToolTip (tooltip);
}

CContentDirectoryBrowser::CContentDirectoryBrowser (QWidget* parent) : CListWidgetBase (parent)
{
  QScrollBar* sb = verticalScrollBar ();
  connect (sb, &QScrollBar::valueChanged, this, &CContentDirectoryBrowser::scrollBarValueChanged);
  connect (this, &QListWidget::itemSelectionChanged, this, &CContentDirectoryBrowser::itemSelectionChanged);
  connect (new QShortcut (QKeySequence (Qt::Key_Escape), this), &QShortcut::activated, this, &CContentDirectoryBrowser::clearSelection);
  connect (new QShortcut (QKeySequence (Qt::Key_Return), this), &QShortcut::activated, this, &CContentDirectoryBrowser::startStream);
  m_iconUpdateTimer.setInterval (m_iconUpdateThresholdTime);
  if (!m_iconUpdateTimer.isSingleShot ())
  {
    m_iconUpdateTimer.setSingleShot (true);
  }
}

int CContentDirectoryBrowser::addItems (QList<CDidlItem> const & didlItems)
{
  if (!didlItems.isEmpty ())
  {
    for (CDidlItem const & didlItem : didlItems)
    {
      CContentDirectoryBrowserItem* item = new CContentDirectoryBrowserItem (didlItem, this);
      item->setIcon (CContentDirectoryBrowserItem::standardIcon (didlItem, m_defaultIcons));
    }

    startIconUpdateTimer ();
  }

  return didlItems.size ();
}

int CContentDirectoryBrowser::addItems (QString const & server,
                                        QString const & id,
                                        CContentDirectory::EBrowseType type,
                                        QString filter,
                                        int startingIndex,
                                        int requestedCount,
                                        QString const & sortCriteria)
{
  if (filter.isEmpty ())
  {
    filter = QLatin1Char ('*');
  }

  clear ();
  CBrowseReply reply;
  CPlugin*     plugin = m_cp->plugin (server);
  if (plugin != nullptr)
  {
    reply = plugin->browse (id, type, filter, startingIndex, requestedCount, sortCriteria);
  }
  else
  {
    CContentDirectory cd (m_cp);
    reply = cd.browse (server, id, type, filter, startingIndex, requestedCount, sortCriteria);
  }

  int numberReturned = reply.numberReturned ();
  if (numberReturned != 0)
  {
    if (sortCriteria == QString::null && m_sortCriteria != QString::null)
    {
      reply.sort (m_sortCriteria, m_sortDir);
    }

    addItems (reply.items ());
  }

  return numberReturned;
}

void CContentDirectoryBrowser::updateIcon ()
{
  stopIconUpdateTimer ();
  if (!m_blockIconUpdate)
  {
    int cItems = count ();
    if (cItems != 0)
    {
      QRect itemsRect = viewport ()->rect ();
      itemsRect.adjust (-10, -10, 10, 10);
      bool        update  = false;
      QSize       offset  = iconSize () / 2;
      int         xOffset = offset.width ();
      int         yOffset = offset.height ();
      QModelIndex top     = indexAt (itemsRect.topLeft () + QPoint (xOffset, yOffset));
      if (top.isValid ())
      {
        int firstRow = top.row ();
        if (firstRow > 0)
        {
          --firstRow;
        }

        QModelIndex bottom   = indexAt (itemsRect.bottomLeft () + QPoint (xOffset, -yOffset));
        int         lastRow  = bottom.isValid () ? bottom.row () : cItems - 1;
        QSize       iconSize = this->iconSize ();
        for (int iItem = firstRow; iItem <= lastRow; ++iItem)
        {
          CContentDirectoryBrowserItem* item = static_cast<CContentDirectoryBrowserItem*>(this->item (iItem));
          if (item->iconType () == CContentDirectoryBrowserItem::IconNotdefined)
          {
            QRect itemRect = visualItemRect (item);
            if (itemsRect.intersects (itemRect))
            {
              update = true;
              item->updateIcon (m_cp, m_pixmapCache, -1, m_defaultIcons, iconSize);
              break;
            }
          }
        }

        if (update)
        {
          m_iconUpdateTimer.setInterval (m_iconUpdateRepeatTime);
          m_iconUpdateTimer.start ();
        }
      }
    }
  }
}

void CContentDirectoryBrowser::resizeEvent (QResizeEvent*)
{
  startIconUpdateTimer ();
}

void CContentDirectoryBrowser::scrollBarValueChanged (int)
{
  startIconUpdateTimer ();
}

void CContentDirectoryBrowser::startIconUpdateTimer ()
{
  stopIconUpdateTimer ();
  if (!m_blockIconUpdate && count () != 0)
  {
    if (m_directoryBrowser != this)
    {
      if (m_directoryBrowser != nullptr)
      {
        disconnect (&m_iconUpdateTimer, &QTimer::timeout, this, &CContentDirectoryBrowser::updateIcon);
      }

      connect (&m_iconUpdateTimer, &QTimer::timeout, this, &CContentDirectoryBrowser::updateIcon);
      m_directoryBrowser = this;
    }

    m_iconUpdateTimer.setInterval (m_iconUpdateThresholdTime);
    m_iconUpdateTimer.start ();
  }
}

void CContentDirectoryBrowser::stopIconUpdateTimer ()
{
  m_iconUpdateTimer.stop ();
}

void CContentDirectoryBrowser::setBold (QString const & uri)
{
  if (!uri.isEmpty ())
  {
    for (int iItem = 0, end = count (); iItem < end; ++iItem)
    {
      CContentDirectoryBrowserItem* item   = static_cast<CContentDirectoryBrowserItem*>(this->item (iItem));
      CDidlItem const &             didl   = item->didlItem ();
      QFont                         font   = item->font ();
      QString                       didURI = replace127_0_0_1 (didl.uri (0));
      if (!didURI.isEmpty () && uri.endsWith (didURI)) // endsWith for plugins.
      {
        if (!font.bold ())
        {
          font.setBold (true);
          item->setFont (font);
          scrollToItem (item);
        }
      }
      else if (font.bold ())
      {
        font.setBold (false);
        item->setFont (font);
      }
    }
  }
}

CDidlItem CContentDirectoryBrowser::didlItem (QString const & uri) const
{
  CDidlItem didlItem;
  for (int iItem = 0, end = count (); iItem < end; ++iItem)
  {
    CContentDirectoryBrowserItem* item = static_cast<CContentDirectoryBrowserItem*>(this->item (iItem));
    CDidlItem const &             didl = item->didlItem ();
    QStringList                   uris = didl.uris ();
    if (std::find (uris.begin (), uris.end (), uri) != uris.end ())
    {
      didlItem = didl;
      break;
    }
  }

  return didlItem;
}

int CContentDirectoryBrowser::nextIndex (bool forward, bool repeat, bool shuffle)
{
  int cItems = count ();
  int index  = boldIndex ();
  if (shuffle)
  {
    index = qrand () % cItems;
  }
  else
  {
    if (forward)
    {
      ++index;
      if (index >= cItems)
      {
        index = repeat ? 0 : -1;
      }
    }
    else
    {
      --index;
      if (index < 0)
      {
        index = repeat ? cItems - 1 : -1;
      }
    }
  }

  return index;
}

void CContentDirectoryBrowser::itemSelectionChanged ()
{
  m_cSelected = 0;
  for (int iItem = 0, cItems = count (); iItem < cItems; ++iItem)
  {
    QListWidgetItem* item = this->item (iItem);
    if (item->isSelected ())
    {
      ++m_cSelected;
    }
  }
}

void CContentDirectoryBrowser::startStream ()
{
  QList<QListWidgetItem*> items = selectedItems ();
  if (!items.isEmpty ())
  {
    emit itemDoubleClicked (items.first ());
  }
}

void CContentDirectoryBrowser::setIconType (CContentDirectoryBrowserItem::EIconType type)
{
  for (int iItem = 0, cItems = count (); iItem < cItems; ++iItem)
  {
    CContentDirectoryBrowserItem* item = static_cast<CContentDirectoryBrowserItem*>(this->item (iItem));
    item->setIconType (type);
  }
}

