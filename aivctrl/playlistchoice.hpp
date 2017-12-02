#ifndef PLAYLISTCHOICE_HPP
#define PLAYLISTCHOICE_HPP

#include "playlist.hpp"
#include <QDialog>

namespace Ui {
  class CPlaylistChoice;
}

class QListWidgetItem;

/*! Dialog box to choose an existing playlist or create a new playlist. */
class CPlaylistChoice : public QDialog
{
  Q_OBJECT

public:
  explicit CPlaylistChoice (QMap<QString, CPlaylist> const & playlists, QString const & title, CPlaylist::EType type,
                            int cItems, QWidget* parent = nullptr);
  ~CPlaylistChoice ();

  QString name () const;

protected slots :
  void on_m_names_itemClicked (QListWidgetItem* item);
  void on_m_names_itemDoubleClicked (QListWidgetItem* item);
  void on_m_ok_clicked ();
  void on_m_cancel_clicked ();

private:
  Ui::CPlaylistChoice* ui;
  QMap<QString, CPlaylist> const & m_playlists;
};

#endif // PLAYLISTCHOICE_HPP
