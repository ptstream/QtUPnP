#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "aivwidgets/widgethelper.hpp"
#include "../upnp/avtransport.hpp"
#include "../upnp/renderingcontrol.hpp"

USING_UPNP_NAMESPACE

void CMainWindow::on_m_previousStackedWidgetIndex_clicked ()
{
  searchAction (false);
  int currentIndex = ui->m_stackedWidget->currentIndex ();
  if (currentIndex == Queue || currentIndex == Playing || currentIndex == Playlist)
  {
    CFolderItem const & item = ui->m_folders->top ();
    ui->m_stackedWidget->setCurrentIndex (item.stackedWidgetIndex ());
  }
  else if (!ui->m_folders->isEmpty ())
  {
    CFolderItem item = ui->m_folders->pop ();
    ui->m_stackedWidget->setCurrentIndex (item.stackedWidgetIndex ());
    updateContentDirectory (item, true);
  }
  else
  {
    if (ui->m_stackedWidget->currentIndex () != Home)
    {
      ui->m_stackedWidget->setCurrentIndex (Home);
    }
  }
}

void CMainWindow::on_m_home_clicked ()
{
  ui->m_stackedWidget->setCurrentIndex (Home);
}

void CMainWindow::on_m_currentQueuePlaying_clicked ()
{
  EStackedWidgetIndex index = ui->m_stackedWidget->currentIndex () != Queue ? Queue : Playing;
  ui->m_stackedWidget->setCurrentIndex (index);
}

void CMainWindow::on_m_play_clicked ()
{
  searchAction (false);
  ui->m_play->setEnabled (false);
  ui->m_play2->setEnabled (false);

  CAVTransport   avt (m_cp);
  CTransportInfo  info           = avt.getTransportInfo (m_renderer);
  QString const & transportState = info.currentTransportState ();
  if (!transportState.isEmpty ())
  { // No error occurs
    bool playing = transportState == "PLAYING";
    bool success;
    if (playing)
    {
      QStringList                 actions = avt.getCurrentTransportActions (m_renderer);
      QStringList::const_iterator begin   = actions.cbegin ();
      QStringList::const_iterator end     = actions.cend ();
      QStringList::const_iterator it      = std::find (begin, end, "Pause");
      if (it != end)
      { // Find Pause invoke the command.
        success = avt.pause (m_renderer);
      }
      else
      { // Pause no found try Stop. If Stop not find assume Pause (probably actions is empty).
        it = std::find (begin, end, "Stop");
        success = it != end ? avt.stop (m_renderer) : avt.pause (m_renderer);
      }

      playing = false;
    }
    else
    {
      success = avt.play (m_renderer);
      playing = true;
    }

    if (success)
    { // No error occurs
      rendererStopped ();
    }
  }
}

void CMainWindow::on_m_play2_clicked ()
{
  on_m_play_clicked ();
}

void CMainWindow::on_m_mute_clicked ()
{
  searchAction (false);
  ui->m_mute->setEnabled (false);
  ui->m_mute2->setEnabled (false);

  CRenderingControl rc (m_cp);
  bool              mute = !rc.getMute (m_renderer);
  rc.setMute (m_renderer, mute);
  muteIcon (mute);

  ui->m_mute->setEnabled (true);
  ui->m_mute2->setEnabled (true);
}

void CMainWindow::on_m_mute2_clicked ()
{
  on_m_mute_clicked ();
}

void CMainWindow::on_m_previous_clicked ()
{
  searchAction (false);
  if (!m_cp->playlistName ().isEmpty ())
  {
    CAVTransport (m_cp).previous (m_renderer);
  }
  else
  {
    nextItem (false);
  }
}

void CMainWindow::on_m_previous2_clicked ()
{
  on_m_previous_clicked ();
}

void CMainWindow::on_m_next_clicked ()
{
  searchAction (false);
  if (!m_cp->playlistName ().isEmpty ())
  {
    CAVTransport (m_cp).next (m_renderer);
  }
  else
  {
    nextItem (true);
  }
}

void CMainWindow::on_m_next2_clicked ()
{
  on_m_next_clicked ();
}

void CMainWindow::on_m_absTime_clicked ()
{
  searchAction (false);
  updatePosition ();
}

void CMainWindow::on_m_currentPlayingIcon_clicked ()
{
  ui->m_stackedWidget->setCurrentIndex (Playing);
}

void CMainWindow::on_m_repeat_clicked ()
{
  searchAction (false);
  m_playMode          = static_cast<EPlayMode>((m_playMode + 1) % (RepeatOne + 1));
  QString newPlayMode = playModeString ();
  m_playMode          = playMode (newPlayMode);
  CAVTransport (m_cp).setPlayMode (m_renderer, newPlayMode);
  applyPlayMode ();
}

void CMainWindow::on_m_shuffle_clicked ()
{
  searchAction (false);
  m_playMode          = m_playMode == Shuffle ? Normal : Shuffle;
  QString newPlayMode = playModeString ();
  CAVTransport (m_cp).setPlayMode (m_renderer, newPlayMode);
  applyPlayMode ();
}

void CMainWindow::on_m_favorite_clicked ()
{
  searchAction (false);
  int row = ui->m_queue->boldIndex ();
  if (row != -1)
  {
    CContentDirectoryBrowserItem* item = static_cast<CContentDirectoryBrowserItem*>(ui->m_queue->item (row));
    if (item != nullptr)
    {
      CDidlItem const &    didlItem = item->didlItem ();
      QString const &       id      = didlItem.id ();
      int                   index   = -1;
      for (TMPlaylists::iterator itPl = m_playlists.begin (), endPl = m_playlists.end (); itPl != endPl && index == -1; ++itPl)
      {
        CPlaylist& playlist = itPl.value ();
        if (playlist.isFavorite ())
        {
          QList<CDidlItem>& items = playlist.items ();
          for (QList<CDidlItem>::const_iterator itItem = items.cbegin (), endItem = items.cend (); itItem != endItem && index == -1; ++itItem)
          {
            CDidlItem const & playlistDidlItem = *itItem;
            if (playlistDidlItem.id () == id)
            {
              index = itItem - items.cbegin ();
              items.removeAt (index);
              ui->m_favorite->setIcon (::resIcon ("favorite"));
            }
          }
        }
      }

      if (index == -1)
      {
        CPlaylist::EType type     = CPlaylist::favoritesType (didlItem);
        CPlaylist*       playlist = favorite (m_playlists, type);
        if (playlist != nullptr)
        {
          playlist->addItem (didlItem);
          ui->m_favorite->setIcon (::resIcon ("favorite_true"));
        }
      }
    }
  }
}
