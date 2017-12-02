#include "devicebrowser.hpp"
#include "widgethelper.hpp"
#include "QShortcut"

USING_UPNP_NAMESPACE

CDeviceBrowserItem::CDeviceBrowserItem (QListWidget* parent, int type) : QListWidgetItem (parent, type)
{
}

CDeviceBrowserItem::CDeviceBrowserItem (CDevice const & device, QStringList const & defaultIcons,
            QListWidget* parent, int type) : QListWidgetItem (parent, type), m_device (device)
{
  setText (device.name ());
  QByteArray     bytes = device.pixmapBytes (nullptr, CDevice::SmResol);
  QPixmap        pxm;
  if (!bytes.isEmpty ())
  {
    pxm.loadFromData (bytes);
    if (!pxm.isNull ())
    {
      setIcon (QIcon (pxm));
    }
  }

  CDevice::EType deviceType = device.type ();
  if (pxm.isNull () && deviceType < defaultIcons.size ())
  {
    pxm.load (::resIconFullPath (defaultIcons[deviceType]));
    setIcon (QIcon (pxm));
  }

  updateTooltip ();
}

CDeviceBrowserItem::CDeviceBrowserItem (CDeviceBrowserItem const & other) :
                  QListWidgetItem (other), m_device (other.m_device)
{
}

void CDeviceBrowserItem::updateTooltip ()
{
  QString string = m_device.name ();
  if (string.isEmpty ())
  {
    string = QObject::tr ("Unknown");
  }

  QString tooltip = TOOLTIPFIELD1 ("Title", string);
  tooltip        += TOOLTIPFIELD ("Url", m_device.url ().toString ());
  tooltip        += TOOLTIPFIELD ("Model name", m_device.modelName ());
  tooltip        += TOOLTIPFIELD ("Model number", m_device.modelNumber ());
  tooltip        += TOOLTIPFIELD ("Model url", m_device.modelURL ());
  tooltip        += TOOLTIPFIELD ("Description", m_device.modelDesc ());
  tooltip        += TOOLTIPFIELD ("Presentation", m_device.presentationURL ());
  tooltip        += TOOLTIPFIELD ("Manufacturer", m_device.manufacturer ());
  tooltip        += TOOLTIPFIELD ("Manufacturer url", m_device.manufacturerURL ());
  tooltip        += TOOLTIPFIELD ("Device type", m_device.deviceType ());
  QString version = QString ("%1.%2").arg (m_device.majorVersion ()).arg (m_device.minorVersion ());
  tooltip        += TOOLTIPFIELD ("Version", version);

  if (m_device.type () == CDevice::MediaRenderer)
  {
    CDevice::EPlaylistStatus status = m_device.playlistStatus ();
    switch (status)
    {
      case CDevice::NoPlaylistHandler :
        string = "no";
        break;

      case CDevice::PlaylistHandler :
        string = "yes";
        break;

      default :
        string = "unknown";
        break;
    }

    tooltip  += TOOLTIPFIELD ("Manage playlists", string);
  }

  setToolTip (tooltip);
}

CDeviceBrowser::CDeviceBrowser (QWidget* parent) : CListWidgetBase (parent)
{
  setTransparentBackGround (this);
  connect (new QShortcut (QKeySequence (Qt::Key_Escape), this), &QShortcut::activated, this, &CListWidgetBase::clearSelection);
}

void CDeviceBrowser::addItem (CDevice const & device)
{
  new CDeviceBrowserItem (device, m_defaultIcons, this);
}

void CDeviceBrowser::delItem (QString const & uuid)
{
  for (int row = 0, cRows = count (); row < cRows; ++row)
  {
    CDeviceBrowserItem* item   = static_cast<CDeviceBrowserItem*>(this->item (row));
    CDevice const &     device = item->device ();
    if (device.uuid () == uuid)
    {
      delete item;
      break;
    }
  }
}

QIcon CDeviceBrowser::icon (QString const & uuid)
{
  QIcon icon;
  for (int row = 0, cRows = count (); row < cRows; ++row)
  {
    CDeviceBrowserItem* item   = static_cast<CDeviceBrowserItem*>(this->item (row));
    CDevice const &     device = item->device ();
    if (device.uuid () == uuid)
    {
      icon = item->icon ();
      break;
    }
  }

  return icon;
}

