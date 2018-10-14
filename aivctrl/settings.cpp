#include "settings.hpp"
#include "ui_settings.h"
#include "aivwidgets/widgethelper.hpp"
#include <QToolButton>
#include <QListWidget>

CSettings::CSettings (bool* flags, QSize& iconSize, QWidget* parent) : QDialog (parent),
        ui (new Ui::CSettings), m_flags (flags), m_iconSize (iconSize)
{
  ui->setupUi (this);
  removeWindowContextHelpButton (this);

  ui->m_networkCom->setChecked (flags[ShowNetworkCom]);
  ui->m_search->setChecked (flags[UseSearchForCheckPlaylist]);
  ui->m_eventOnly->setChecked (flags[UPnPEventsOnly]);
  ui->m_playlists->setChecked (flags[DontUsePlaylists]);
  ui->m_cloudServers->setChecked (flags[ShowCloudServers]);
  ui->m_iconSize->setValue (iconSize.width ());
  m_iconSizeBackup = iconSize;
  ui->m_size->setText (QString::number (iconSize.width ()));
}

CSettings::~CSettings ()
{
  delete ui;
}

void CSettings::on_m_iconSize_valueChanged (int value)
{
  QWidget* mw = static_cast<QWidget*>(parent ());
  m_iconSize  = QSize (value, value);
  ui->m_size->setText (QString::number (value));
  setIconSize (mw, m_iconSize);
}

void CSettings::setIconSize (QWidget* w, QSize const & size)
{
  QList<QToolButton*> tbs = w->findChildren<QToolButton*> ();
  for (QToolButton* tb : tbs)
  {
    tb->setIconSize (size);
  }

  QList<QListWidget*> listWidgets = w->findChildren<QListWidget*> ();
  for (QListWidget* listWidget : listWidgets)
  {
    listWidget->setIconSize (size);
    listWidget->setSpacing (1);
  }
}

void CSettings::on_m_reset_clicked ()
{
  ui->m_networkCom->setChecked (true);
  ui->m_search->setChecked (false);
  ui->m_eventOnly->setChecked (false);
  ui->m_playlists->setChecked (false);
  ui->m_cloudServers->setChecked (false);
  ui->m_iconSize->setValue (32);
}

void CSettings::on_m_ok_clicked ()
{
  m_flags[ShowNetworkCom]            = ui->m_networkCom->isChecked ();
  m_flags[UseSearchForCheckPlaylist] = ui->m_search->isChecked ();
  m_flags[UPnPEventsOnly]            = ui->m_eventOnly->isChecked ();
  m_flags[DontUsePlaylists]          = ui->m_playlists->isChecked ();
  m_flags[ShowCloudServers]          = ui->m_cloudServers->isChecked ();
  accept ();
}

void CSettings::on_m_cancel_clicked ()
{
  on_m_iconSize_valueChanged (m_iconSizeBackup.width ());
  reject ();
}
