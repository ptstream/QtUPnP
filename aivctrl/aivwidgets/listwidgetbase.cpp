#include "listwidgetbase.hpp"
#include "widgethelper.hpp"

CListWidgetBase::CListWidgetBase (QWidget* parent) : QListWidget (parent)
{
  setTransparentBackGround (this);
  connect (this, &CListWidgetBase::itemClicked, this, &CListWidgetBase::saveSelected);
  connect (this, &CListWidgetBase::itemDoubleClicked, this, &CListWidgetBase::saveSelected);
}

void CListWidgetBase::saveSelected (QListWidgetItem* item)
{
  m_selectedRow = row (item);
}

int CListWidgetBase::boldIndex () const
{
  int index = -1;
  for (int iItem = 0, end = count (); iItem < end; ++iItem)
  {
    QListWidgetItem* item = this->item (iItem);
    QFont            font = item->font ();
    if (font.bold ())
    {
      index = iItem;
      break;
    }
  }

  return index;
}
