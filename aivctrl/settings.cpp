#include "settings.hpp"
#include "ui_settings.h"
#include "aivwidgets/widgethelper.hpp"

CSettings::CSettings (bool* flags, QWidget* parent) : QDialog (parent), ui (new Ui::CSettings),
                                         m_flags (flags)
{
  ui->setupUi (this);
  removeWindowContextHelpButton (this);

  ui->m_networkCom->setChecked (flags[ShowNetworkCom]);
  ui->m_search->setChecked (flags[UseSearchForCheckPlaylist]);
  ui->m_eventOnly->setChecked (flags[UPnPEventsOnly]);
}

CSettings::~CSettings ()
{
  delete ui;
}

void CSettings::on_m_reset_clicked ()
{
  ui->m_networkCom->setChecked (true);
  ui->m_search->setChecked (false);
  ui->m_eventOnly->setChecked (false);
}

void CSettings::on_m_ok_clicked ()
{
  m_flags[ShowNetworkCom]            = ui->m_networkCom->isChecked ();
  m_flags[UseSearchForCheckPlaylist] = ui->m_search->isChecked ();
  m_flags[UPnPEventsOnly]            = ui->m_eventOnly->isChecked ();
  accept ();
}

void CSettings::on_m_cancel_clicked ()
{
  reject ();
}
