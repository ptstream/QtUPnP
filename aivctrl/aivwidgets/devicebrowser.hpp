#ifndef DEVICEBROWSER_HPP
#define DEVICEBROWSER_HPP

#include "listwidgetbase.hpp"
#include "../upnp/device.hpp"

/*! \brief This class adds CDevice at QListWidgetItem used to display devices.
 * It is used to show servers.
 */
class CDeviceBrowserItem : public QListWidgetItem
{
public :
  /*! Default constructor. */
  CDeviceBrowserItem (QListWidget* parent = nullptr, int type = Type);

  /*! Constructor with device and default icons. */
  CDeviceBrowserItem (QtUPnP::CDevice const & device, QStringList const & defaultIcons, QListWidget* parent = nullptr, int type = Type);

  /*! Copy constructor. */
  CDeviceBrowserItem (CDeviceBrowserItem const & other);

  /*! Sets the device to the item. */
  void setDevice (QtUPnP::CDevice const & device) { m_device = device; }

  /*! Returns the device of the item. */
  QtUPnP::CDevice const & device () const { return m_device; }

  /*! Update the tooltip. */
  void updateTooltip ();

private :
  QtUPnP::CDevice m_device; //!< The device.
};

/*! \brief The CDeviceBrowser class provides an CDeviceBrowserItem list widget. */
class CDeviceBrowser : public CListWidgetBase
{
  Q_OBJECT

public:
  /*! Default constructor. */
  CDeviceBrowser (QWidget* parent = nullptr);

  /*! Adds a device to the list widget. */
  void addItem (QtUPnP::CDevice const & device);

  /*! Deletes a device of the list widget. */
  void delItem (QString const & uuid);

  QIcon icon (QString const & uuid);
};

#endif // DEVICEBROWSER_HPP
