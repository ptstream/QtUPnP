#ifndef ITEM_HPP
#define ITEM_HPP

#include "../upnp/didlitem.hpp"
#include <QTreeWidgetItem>

USING_UPNP_NAMESPACE

enum EItemType { NoType = QTreeWidgetItem::Type,
                 BrowseType = QTreeWidgetItem::UserType,
                 SearchType,
                 ActionType,
               };

class CItem : public QTreeWidgetItem
{
public :
  CItem (QTreeWidgetItem* item, CDidlItem const & didl, EItemType type);

  CDidlItem const & didl () const { return m_didl; }
  CDidlItem& didl () { return m_didl; }

private :
  CDidlItem m_didl;
};

#endif // ITEM_HPP
