#include "folderbrowser.hpp"
#include "listwidgetbase.hpp"
#include <QMouseEvent>

struct SFolderItemData : public QSharedData
{
  SFolderItemData () : QSharedData () {}
  SFolderItemData (int stackedWidgetIndex, QString const & name,
                   QString const & parentID = QString::null,
                   QString const & id = QString::null,
                   CListWidgetBase* listWidget = nullptr);
  SFolderItemData (SFolderItemData const & other);

  int m_stackedWidgetIndex = 0;
  QString m_name;
  QString m_parentID;
  QString m_id;
  CListWidgetBase* m_listWidget = nullptr;
  int m_row = -1;
};

SFolderItemData::SFolderItemData (SFolderItemData const & other) :  QSharedData (other),
   m_stackedWidgetIndex (other.m_stackedWidgetIndex), m_name (other.m_name), m_parentID (other.m_parentID),
   m_id (other.m_id), m_listWidget (other.m_listWidget), m_row (other.m_row)
{
}

SFolderItemData::SFolderItemData (int stackedWidgetIndex, QString const & name,
   QString const & parentID, QString const & id, CListWidgetBase* listWidget) :  QSharedData (),
   m_stackedWidgetIndex (stackedWidgetIndex), m_name (name), m_parentID (parentID), m_id (id),
   m_listWidget (listWidget)
{
  if (m_listWidget != nullptr)
  {
    m_row = m_listWidget->selectedRow ();
  }
}

CFolderItem::CFolderItem () : m_d (new SFolderItemData)
{
}

CFolderItem::CFolderItem (int index, QString const & name, QString const & parentID,
          QString const & id, CListWidgetBase* listWidget) :
          m_d (new SFolderItemData (index, name, parentID, id, listWidget))
{
}

CFolderItem::CFolderItem (CFolderItem const & rhs) : m_d (rhs.m_d)
{
}

CFolderItem& CFolderItem::operator = (CFolderItem const & rhs)
{
  if (this != &rhs)
  {
    m_d.operator = (rhs.m_d);
  }

  return *this;
}

CFolderItem::~CFolderItem ()
{
}

void CFolderItem::setStackedWidgetIndex (int index)
{
  m_d->m_stackedWidgetIndex = index;
}

void CFolderItem::setID (QString const & id)
{
  m_d->m_id = id;
}

void CFolderItem::setParentID (QString const & id)
{
  m_d->m_parentID = id;
}

void CFolderItem::setListWidget (CListWidgetBase* listWidget)
{
  m_d->m_listWidget = listWidget;
}

void CFolderItem::setRow (int row)
{
  m_d->m_row = row;
}

void CFolderItem::setName (QString const & name)
{
  m_d->m_name = name;
}

int CFolderItem::stackedWidgetIndex () const
{
  return m_d->m_stackedWidgetIndex;
}

QString const & CFolderItem::id () const
{
  return m_d->m_id;
}

QString const & CFolderItem::parentID () const
{
  return m_d->m_parentID;
}

CListWidgetBase* CFolderItem::listWidget () const
{
  return m_d->m_listWidget;
}

int CFolderItem::row () const
{
  return m_d->m_row;
}

QString const & CFolderItem::name () const
{
  return m_d->m_name;
}

void CFolderItem::setListWidgetCurrentItem () const
{
  if (m_d->m_listWidget != nullptr && m_d->m_row != -1)
  {
    QListWidgetItem* item = m_d->m_listWidget->item (m_d->m_row);
    if (item != nullptr)
    {
      item->setSelected (true);
      m_d->m_listWidget->scrollToItem (item);
    }
  }
}

CFolderBrowser::CFolderBrowser (QWidget* parent) : QLineEdit (parent)
{
}

bool CFolderBrowser::findFolder (int& start, int& end, int pos)
{
  bool            find = false;
  QString const & text = this->text ();
  int             tlen = text.length ();
  int             slen = m_separator.length ();
  if (pos >= 0 && pos < tlen)
  {
    start = end = pos;
    while (start >= 1 && text.mid (start, slen) != m_separator)
    {
      --start;
    }

    if (text.indexOf (m_separator, start) == start)
    {
      start += slen;
    }

    while (end < tlen && text.mid (end, slen) != m_separator)
    {
      ++end;
    }

    find = true;
  }

  return find;
}

void CFolderBrowser::mouseMoveEvent (QMouseEvent* event)
{
  QPoint cursorPos = event->pos ();
  int    pos       = cursorPositionAt (cursorPos);
  int    start, end;
  if (findFolder (start, end, pos))
  {
    deselect ();
    m_selectedFolder = false;
    if (end != text ().length ())
    {
      setSelection (start, end - start);
      m_selectedFolder = true;
    }
  }
}

void CFolderBrowser::mouseReleaseEvent (QMouseEvent* event)
{
  if (m_selectedFolder)
  {
    QPoint  cursorPos = event->pos ();
    int     pos       = cursorPositionAt (cursorPos);
    QString text      = this->text ().left (pos);
    int     index     = text.count (m_separator) + 2;
    if (index >= m_items.size ())
    {
      index = 0;
    }

    emit indexSelected (index);
  }
}

void CFolderBrowser::enterEvent (QEvent*)
{
  deselect ();
  m_selectedFolder = false;
}

void CFolderBrowser::leaveEvent (QEvent*)
{
  deselect ();
  m_selectedFolder = false;
}

void CFolderBrowser::updateText ()
{
  QString text;
  for (QStack<CFolderItem>::const_iterator it = m_items.begin (), end = m_items.end (); it != end; ++it)
  {
    QString name = (*it).name ();
    if (!text.isEmpty ())
    {
      text += m_separator;
    }

    text += name;
  }

  setText (text);
  setToolTip (text);
}

void CFolderBrowser::push (int index, QString name, QString const & parentID,
                           QString const & id, CListWidgetBase* listWidget)
{
  m_items.push (CFolderItem (index, name, parentID, id, listWidget));
  if (name.startsWith (m_slash))
  {
    setText (name.remove (0, 1));
    setText (name);
  }
  else
  {
    updateText ();
  }
}

CFolderItem CFolderBrowser::pop ()
{
  CFolderItem item = m_items.pop ();
  updateText ();
  return item;
}

void CFolderBrowser::clearStack ()
{
  if (m_items.size () > 1)
  {
    CFolderItem const & item       = m_items.first ();
    QListWidget*        listWidget = item.listWidget ();
    int                 row        = item.row ();
    if (listWidget != nullptr && row != -1)
    {
      listWidget->setCurrentRow (row);
    }

    m_items.erase (m_items.begin () + 1, m_items.end ());
  }

  QLineEdit::clear ();
}

bool CFolderBrowser::erase (int start, int end)
{
  bool success = !m_items.isEmpty () && start >= 0 && start < m_items.size ();
  if (success)
  {
    QStack<CFolderItem>::iterator itStart = m_items.begin () + start;
    QStack<CFolderItem>::iterator itEnd   = end == -1 || end > m_items.size () ?
                                            m_items.end () : m_items.begin () + end;
    m_items.erase (itStart, itEnd);
    updateText ();
  }

  return success;
}

