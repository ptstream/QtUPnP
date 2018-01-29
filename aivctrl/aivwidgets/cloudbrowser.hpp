#ifndef CLOUDBROWSER_HPP
#define CLOUDBROWSER_HPP

#include "listwidgetbase.hpp"

namespace QtUPnP
{
  class CPlugin;
}

class CCloudBrowserItem : public QListWidgetItem
{
public :
  /*! Default constructor. */
  CCloudBrowserItem (QListWidget* parent = nullptr, int type = Type);

  /*! Constructor with device and default icons. */
  CCloudBrowserItem (QtUPnP::CPlugin const * plugin, QListWidget* parent = nullptr, int type = Type);

  /*! Copy constructor. */
  CCloudBrowserItem (CCloudBrowserItem const & other);

  /*! Update the tooltip. */
  void updateTooltip ();

  QtUPnP::CPlugin const * plugin () const { return m_plugin; }

private :
  QtUPnP::CPlugin const * m_plugin;
};

class CCloudBrowser : public CListWidgetBase
{
  Q_OBJECT

public:
  CCloudBrowser (QWidget* parent = nullptr);

  /*! Adds a device to the list widget. */
  void addItem (QtUPnP::CPlugin const * plugin);
};

#endif // CLOUDBROWSER_HPP
