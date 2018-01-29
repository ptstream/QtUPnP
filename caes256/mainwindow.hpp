#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

namespace Ui {
  class CMainWindow;
}

class CMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit CMainWindow(QWidget *parent = 0);
  ~CMainWindow();

protected slots :
  void on_m_crypt_clicked ();

private:
  Ui::CMainWindow *ui;
};

#endif // MAINWINDOW_HPP
