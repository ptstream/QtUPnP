#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "../upnp/browsereply.hpp"
#include <QDialog>

USING_UPNP_NAMESPACE

namespace Ui {
  class CSearch;
}

class CSearch : public QDialog
{
  Q_OBJECT

public:
  explicit CSearch (QList<CDidlItem> const & items, QWidget *parent = 0);
  ~CSearch ();

protected slots :
  void on_m_text_textChanged (QString const & text);

private:
  Ui::CSearch* ui;
  QList<CDidlItem> m_items;
};

#endif // SEARCH_HPP
