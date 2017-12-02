#ifndef ABOUT_HPP
#define ABOUT_HPP

#include <QDialog>

namespace Ui {
  class CAbout;
}

/*! The about dialog. */
class CAbout : public QDialog
{
  Q_OBJECT

public:
  explicit CAbout (QWidget* parent = 0);
  ~CAbout ();

protected slots :
  void on_m_close_clicked ();

private:
  Ui::CAbout* ui;
};

#endif // ABOUT_HPP
