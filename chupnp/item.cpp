#include "item.hpp"

CItem::CItem (QTreeWidgetItem* item, CDidlItem const & didl, EItemType type) :
    QTreeWidgetItem (item, type), m_didl (didl)
{
  CDidlElem const & elem  = didl.value ("dc:title");
  QString           title = elem.value ();
  setText (0, title);
}
