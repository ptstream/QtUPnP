#include "search.hpp"
#include "ui_search.h"

CSearch::CSearch (QList<CDidlItem> const & items, QWidget *parent) : QDialog (parent),
                       ui (new Ui::CSearch), m_items (items)
{
  ui->setupUi (this);
  for (CDidlItem const & item : items)
  {
    QString const & title = item.title ();
    ui->m_items->addItem (title);
  }
}

CSearch::~CSearch ()
{
  delete ui;
}

void CSearch::on_m_text_textChanged (QString const & text)
{
  QList<CDidlItem> sortedItems;
  ui->m_items->clear ();
  if (text.isEmpty ())
  {
    sortedItems = m_items;
  }
  else
  {
    sortedItems = CBrowseReply::search (m_items, text, 30);
  }

  for (CDidlItem const & item : sortedItems)
  {
    QString const & title = item.title ();
    ui->m_items->addItem (title);
  }

  ui->m_items->scrollToTop ();
}
