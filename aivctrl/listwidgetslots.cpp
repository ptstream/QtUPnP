#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "../upnp/plugin.hpp"

USING_UPNP_NAMESPACE

void CMainWindow::on_m_servers_itemClicked (QListWidgetItem* item)
{
  QApplication::setOverrideCursor (Qt::WaitCursor);
  ui->m_myDevice->clearSelection ();
  QString const       rootID ("0");
  CDeviceBrowserItem* dbItem = static_cast<CDeviceBrowserItem*>(item);
  CDevice const &     device = dbItem->device ();
  m_server                   = device.uuid ();
  ui->m_contentDirectory->addItems (m_server, rootID);
  ui->m_stackedWidget->setCurrentIndex (ContentDirectory);
  ui->m_folders->push (Home, device.name (),  QString (), rootID, ui->m_servers);
  ui->m_provider->setText (ui->m_serverLabel->text ());
  if (m_status.hasStatus (ShowNetworkCom))
  {
    setComServerIcon ();
  }

  QApplication::restoreOverrideCursor ();
}

void CMainWindow::on_m_cloud_itemClicked (QListWidgetItem* item)
{
  QApplication::setOverrideCursor (Qt::WaitCursor);
  ui->m_cloud->clearSelection ();
  QString const      rootID ("0");
  CCloudBrowserItem* cbItem = static_cast<CCloudBrowserItem*>(item);
  CPlugin const *    plugin = cbItem->plugin ();
  m_server                  = plugin->uuid ();
  ui->m_contentDirectory->addItems (m_server, rootID);
  ui->m_stackedWidget->setCurrentIndex (ContentDirectory);
  ui->m_folders->push (Home, plugin->friendlyName (),  QString (), rootID, ui->m_servers);
  ui->m_provider->setText (ui->m_serverLabel->text ());
  if (m_status.hasStatus (ShowNetworkCom))
  {
    setComServerIcon ();
  }

  QApplication::restoreOverrideCursor ();
}

void CMainWindow::on_m_contentDirectory_itemDoubleClicked (QListWidgetItem* item)
{
  searchAction (false);
  QApplication::setOverrideCursor (Qt::WaitCursor);
  QString                       title;
  CContentDirectoryBrowser*     lw       = static_cast<CContentDirectoryBrowser*>(item->listWidget ());
  CContentDirectoryBrowserItem* cdItem   = static_cast<CContentDirectoryBrowserItem*>(item);
  CDidlItem const &             didlItem = cdItem->didlItem ();
  CDidlItem::EType              type     = didlItem.type ();
  QString                       itemID, parentID;
  int                           count    = 0;
  if (type == CDidlItem::Unknown || didlItem.isContainer ())
  { // It is a container (assume CDidlItem::Unknown (AssetUPnP) is a container).
    parentID = didlItem.containerParentID ();
    title    = didlItem.title ();
    itemID   = didlItem.containerID ();
    count    = lw->addItems (m_server, itemID);
    if (count != 0)
    {
      lw->scrollToTop ();
    }

    ui->m_folders->push (ContentDirectory, title, parentID, itemID, lw);
  }
  else
  { // It is an item.
    count    = lw->count ();
    QList<CDidlItem> items;
    items.reserve (count);
    for (int iItem = 0; iItem < count; ++iItem)
    {
      CContentDirectoryBrowserItem* cdItem = static_cast<CContentDirectoryBrowserItem*>(lw->item (iItem));
      items.push_back (cdItem->didlItem ());
    }

    ui->m_queue->clear ();
    ui->m_queue->addItems (items);
    m_cp->clearPlaylist ();
    setAVTransport (cdItem);
    ui->m_stackedWidget->setCurrentIndex (Queue);
  }

  QApplication::restoreOverrideCursor ();
}

void CMainWindow::on_m_playlistContent_itemDoubleClicked (QListWidgetItem* item)
{
  on_m_contentDirectory_itemDoubleClicked (item);
}

void CMainWindow::on_m_queue_itemDoubleClicked (QListWidgetItem* item)
{
  searchAction (false);
  CContentDirectoryBrowserItem* cdItem = static_cast<CContentDirectoryBrowserItem*>(item);
  CDidlItem const &             didl   = cdItem->didlItem ();
  if (didl.type () != CDidlItem::Unknown)
  {
    QString itemID = didl.itemID ();
    if (!itemID.isEmpty ())
    {
      QApplication::setOverrideCursor (Qt::WaitCursor);
      setAVTransport (cdItem);
      QApplication::restoreOverrideCursor ();
    }
  }
}

void CMainWindow::on_m_myDevice_itemClicked (QListWidgetItem* item)
{
  CMyDeviceBrowserItem* dbItem = static_cast<CMyDeviceBrowserItem*>(item);
  if (dbItem->isContainer ())
  {
    CMainWindow::EStatus status;
    switch (dbItem->type ())
    {
      case CMyDeviceBrowserItem::AudioContainer :
        status = CMainWindow::CollapseAudioPlaylist;
        break;

      case CMyDeviceBrowserItem::ImageContainer :
        status = CMainWindow::CollapseImagePlaylist;
        break;

      default :
        status = CMainWindow::CollapseVideoPlaylist;
        break;
    }

    bool collapse = ui->m_myDevice->collapseExpand (dbItem);
    if (collapse)
    {
      m_status.addStatus (status);
    }
    else
    {
      m_status.remStatus (status);
    }
  }
}

void CMainWindow::on_m_myDevice_itemDoubleClicked (QListWidgetItem* item)
{
  ui->m_provider->setText (ui->m_myDeviceLabel->text ());
  CMyDeviceBrowserItem* mdbItem = static_cast<CMyDeviceBrowserItem*>(item);
  if (mdbItem->isContainer ())
  {
    ui->m_myDevice->containerDoubleClicked (mdbItem);
  }
  else
  {
    QString           name     = mdbItem->playlistName ();
    CPlaylist const & playlist = m_playlists.value (name);
    ui->m_playlistContent->clear ();
    ui->m_playlistContent->addItems (playlist.items ());
    ui->m_playlistContent->setName (name);
    ui->m_stackedWidget->setCurrentIndex (Playlist);
    CMyDeviceBrowserItem* containerItem = ui->m_myDevice->containerItem (mdbItem);
    QString               text          = containerItem->playlistName () + ui->m_folders->separator () + name;
    ui->m_folders->setText (text);
  }
}

void CMainWindow::newPlaylist (QString const & name, int type)
{
  CPlaylist playlist (static_cast<CPlaylist::EType>(type));
  m_playlists.insertMulti (name, playlist);
}

void CMainWindow::renamePlaylist (QString const & name, const QString& newName)
{
  if (name != newName)
  {
    CPlaylist playlist = m_playlists.take (name);
    playlist.setChanged ();
    m_playlists.insert (newName, playlist);
  }
}

void CMainWindow::removePlaylist (QString const & name)
{
  m_playlists.remove (name);
}

void CMainWindow::rowsMoved (QModelIndex const &, int start, int end, QModelIndex const &, int row)
{
  searchAction (false);
  CPlaylistBrowser* sender = static_cast<CPlaylistBrowser*>(this->sender ());
  if (sender == ui->m_playlistContent)
  {
    QString const & name     = sender->name ();
    CPlaylist&      playlist = m_playlists[name];
    playlist.rowsMoved (start, end, row);
    sender->decSelectedCount ();
  }
  else if (sender == ui->m_queue)
  {
    sender->decSelectedCount ();
    if (sender->selectedCount () == 0)
    {
      currentQueueChanged ();
    }
  }
}

void CMainWindow::removeIDs (QString const & name, QStringList const & ids)
{
  if (name.isEmpty ())
  {
    currentQueueChanged ();
  }
  else
  {
    m_playlists[name].remItems (ids);
  }
}

