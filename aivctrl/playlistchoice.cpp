#include "playlistchoice.hpp"
#include "helper.hpp"
#include "ui_playlistchoice.h"
#include "aivwidgets/widgethelper.hpp"

CPlaylistChoice::CPlaylistChoice (QMap<QString, CPlaylist> const & playlists, QString const & title, CPlaylist::EType type, int cItems, QWidget* parent) :
           QDialog (parent), ui (new Ui::CPlaylistChoice), m_playlists (playlists)
{
  removeWindowContextHelpButton (this);
  ui->setupUi (this);
  setTransparentBackGround (ui->m_name);
  setTransparentBackGround (ui->m_names);

  QString text = tr ("Playlist name for %1 items");
  text = text.replace ("%1", QString::number (cItems));
  ui->m_label->setText (text);

  setWindowTitle (title);

  QIcon icon = ::resIcon ("playlist");
  for (QMap<QString, CPlaylist>::const_iterator it = playlists.cbegin (), end = playlists.cend (); it != end; ++it)
  {
    CPlaylist const & playlist = it.value ();
    if (playlist.type () == type)
    {
      new QListWidgetItem (icon, it.key (), ui->m_names);
    }
  }
}

CPlaylistChoice::~CPlaylistChoice ()
{
  delete ui;
}

QString CPlaylistChoice::name () const
{
  return ui->m_name->text ();
}

void CPlaylistChoice::on_m_names_itemClicked (QListWidgetItem* item)
{
  ui->m_name->setText (item->text ());
}

void CPlaylistChoice::on_m_names_itemDoubleClicked (QListWidgetItem* item)
{
  ui->m_name->setText (item->text ());
  accept ();
}

void CPlaylistChoice::on_m_ok_clicked ()
{
  accept ();
}

void CPlaylistChoice::on_m_cancel_clicked ()
{
  reject ();
}


