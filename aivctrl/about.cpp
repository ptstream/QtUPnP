#include "about.hpp"
#include "ui_about.h"
#include "aivwidgets/widgethelper.hpp"

CAbout::CAbout (QWidget* parent) : QDialog (parent), ui (new Ui::CAbout)
{
  removeWindowContextHelpButton (this);
  ui->setupUi (this);
}

CAbout::~CAbout ()
{
  delete ui;
}

void CAbout::on_m_close_clicked ()
{
  accept ();
}
