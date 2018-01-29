#include "cloudbrowser.hpp"
#include "../upnp/plugin.hpp"

USING_UPNP_NAMESPACE

CCloudBrowserItem::CCloudBrowserItem (QtUPnP::CPlugin const * plugin, QListWidget* parent, int type) :
          QListWidgetItem (parent, type), m_plugin (plugin)
{
  setText (plugin->friendlyName ());
  setIcon (plugin->pixmap ());
}

CCloudBrowser::CCloudBrowser (QWidget* parent) : CListWidgetBase (parent)
{
}

void CCloudBrowser::addItem (CPlugin const * plugin)
{
  new CCloudBrowserItem (plugin, this);
}
