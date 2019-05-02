#include "mainwindow.hpp"
#include "playlistchoice.hpp"
#include "serverscanner.hpp"
#include "about.hpp"
#include "language.hpp"
#include "settings.hpp"
#include "helper.hpp"
#include "ui_mainwindow.h"
#include "aivwidgets/widgethelper.hpp"
#include "aivwidgets/networkprogress.hpp"
#include "../upnp/connectionmanager.hpp"
#include "../upnp/renderingcontrol.hpp"
#include "../upnp/avtransport.hpp"
#include "../upnp/pixmapcache.hpp"
#include <QMenu>
#include <QProgressDialog>
#include <QProgressBar>
#include <QTranslator>

USING_UPNP_NAMESPACE

void CMainWindow::addRendererPopupMenu ()
{
  QMenu* menu = new QMenu (this);
  ui->m_renderer->setMenu (menu);
  ui->m_renderer->setPopupMode (QToolButton::InstantPopup);
  connect (menu, &QMenu::aboutToShow, this, &CMainWindow::aboutToShowRenderer);
  connect (menu, &QMenu::triggered, this, &CMainWindow::rendererAction);
}

void CMainWindow::aboutToShowRenderer ()
{
  // Devices are sorted by alphabetic order.
  auto compare = [this] (QString const & uuid1, QString const & uuid2)
  {
    QString name1 = m_cp->device (uuid1).name ().toLower ();
    QString name2 = m_cp->device (uuid2).name ().toLower ();
    return name1 < name2;
  };

  // Save the actual device icons.
  QMenu*               menu = ui->m_renderer->menu ();
  QMap<QString, QIcon> currentActions;
  QList<QAction*>      actions = menu->actions ();
  for (QAction const * action : actions)
  {
    QString renderer = action->data ().toString ();
    currentActions.insert (renderer, action->icon ());
  }

  menu->clear ();

  // For each renderer, create an action.
  QStringList uuids = m_cp->renderers ();
  std::sort (uuids.begin (), uuids.end (), compare);
  for (QString const & uuid : uuids)
  {
    CDevice const &    device = m_cp->device (uuid);
    QAction*           action = new QAction (device.name (), menu);
    QIcon              icon;
    if (currentActions.contains (uuid))
    { // Restore icon if its saved.
      icon = currentActions.value (uuid);
    }
    else
    {
      QByteArray const & pxmBytes = device.pixmapBytes (m_cp->networkAccessManager (), CDevice::SmResol);
      QPixmap            pxm;
      if (pxmBytes.isEmpty ())
      {
        pxm.load (::resIconFullPath ("device"));
      }
      else
      {
        pxm.loadFromData (pxmBytes);
      }

      icon = QIcon (pxm);
    }

    action->setIcon (icon);
    action->setData (uuid);
    menu->addAction (action);
  }
}

void CMainWindow::rendererAction (QAction* action)
{
  QString renderer = action->data ().toString ();
  if (m_renderer != renderer)
  {
    // Juste to update variables. Normally eventing must update correctly variable.
    // If protocols is empty the renderer not able to receive or send data. May be it is off or crashed.
    QVector<QStringList> protocols = CConnectionManager (m_cp).getProtocolInfos (renderer);
    if (!protocols.isEmpty ())
    {
      if (!m_renderer.isEmpty ())
      {
        m_cp->unsubscribe (m_renderer);
      }

      m_renderer             = renderer;
      CDevice const & device = m_cp->device (renderer);
      QString         name   = device.name ();
      ui->m_rendererName->setText (name);
      ui->m_rendererName->setToolTip (name);
      ui->m_renderer->setIcon (action->icon ());
      if (!m_status.hasStatus (UPnPEventsOnly))
      {
        m_cp->clearPlaylist ();
        CStateVariable const & var     = m_cp->stateVariable (m_renderer,  QString (), "Volume");
        int                    maximum = static_cast<int>(var.maximum ());
        if (maximum <= 0)
        {
          maximum = 100;
        }

        ui->m_volume->setMaximum (maximum);
        ui->m_volume2->setMaximum (maximum);
#ifdef Q_OS_MACOS
        ui->m_volume->setTickInterval (maximum / 10);
        ui->m_volume2->setTickInterval (maximum / 10);
#endif
        CRenderingControl rc (m_cp);
        int volume = rc.getVolume (m_renderer);
        updateVolumeSlider (volume, true);

        bool mute = rc.getMute (m_renderer);
        muteIcon (mute);

        CAVTransport avt (m_cp);
        CTransportSettings transportSettings = avt.getTransportSettings (m_renderer);
        m_playMode                           = playMode (transportSettings.playMode ());
        applyPlayMode ();

        CTransportInfo transportInfo = avt.getTransportInfo (m_renderer);
        bool playing = transportInfo.currentTransportState () == "PLAYING";
        playingIcon (playing);

        // First try to get informations from GetMediaInfo action.
        CMediaInfo mediaInfo     = avt.getMediaInfo (m_renderer);
        CDidlItem  didlMediaInfo = mediaInfo.currentURIDidlItem ();
        if (didlMediaInfo.albumArtURIs ().isEmpty ())
        { // No albumArt, try to get informations from GetPositionInfo action.
          CPositionInfo positionInfo     = avt.getPositionInfo (m_renderer);
          CDidlItem     didlPositionInfo = positionInfo.didlItem ();
          didlMediaInfo                  = CDidlItem::mix (didlMediaInfo, didlPositionInfo);
          if (didlMediaInfo.albumArtURIs ().isEmpty ())
          { // No albumArt, try to get informations from current queue.
            QString uri = didlMediaInfo.uri (0);
            if (!uri.isEmpty ())
            {
              CDidlItem didlItem = ui->m_queue->didlItem (uri);
              didlMediaInfo      = CDidlItem::mix (didlMediaInfo, didlItem);
              if (didlMediaInfo.albumArtURIs ().isEmpty ())
              { // No albumArt, try to get informations from current content directory.
                didlItem      = ui->m_contentDirectory->didlItem (uri);
                didlMediaInfo = CDidlItem::mix (didlMediaInfo, didlItem);
              }
            }
          }
        }

        updateCurrentPlayling (didlMediaInfo, playing, true);
      }

      m_cp->subscribe (renderer);
      setComRendererIcon ();
    }
  }
}

void CMainWindow::addContextualActions (QListWidget* lw, QMenu* menu)
{
  if (lw != nullptr)
  {
    QList<QListWidgetItem*> selectedItems  = lw->selectedItems ();
    int                     cSelectedItems = selectedItems.size ();
    menu->clear ();
    if (lw == ui->m_contentDirectory || lw == ui->m_queue || lw == ui->m_playlistContent)
    {
      char const * iconName = !ui->m_provider->isReadOnly () ? "search_cancel" : "search";
      QString      title    = tr ("Search");
      QAction*     action   = menu->addAction (::resIcon (iconName), title);
      action->setData (Search);

      if (cSelectedItems > 0)
      {
        menu->addSeparator ();

        action = menu->addAction (::resIcon ("replace_queue"), tr ("Replace the queue"));
        action->setData (ReplaceQueue);
        action = menu->addAction (::resIcon ("add_queue"), tr ("Add to the queue"));
        action->setData (AddQueue);
        action->setEnabled (cSelectedItems > 0);

        menu->addSeparator ();

        action = menu->addAction (::resIcon ("add_playlist"), tr ("Add to a playlist"));
        action->setData (AddPlaylist);

        menu->addSeparator ();

        action = menu->addAction (::resIcon ("add_favorites"), tr ("Add to favorites"));
        action->setData (AddFavorites);

        menu->addSeparator ();

        if (lw != ui->m_contentDirectory)
        {
          menu->addSeparator ();
          action = menu->addAction (::resIcon ("delete"), tr ("Remove"));
          action->setData (RemoveTracks);
        }
      }
    }
    else if (ui->m_myDevice)
    {
      QAction* action = menu->addAction (::resIcon ("network16"), tr ("Look for devices"));
      action->setData (ScanNetwork);
      if (cSelectedItems == 1)
      {
        CMyDeviceBrowserItem* mdbItem = static_cast<CMyDeviceBrowserItem*>(selectedItems.first ());
        if (mdbItem->isUserPlaylist ())
        {
          menu->addSeparator ();
          action = menu->addAction (::resIcon ("rename"), tr ("Rename"));
          action->setData (RenamePlaylist);
          action = menu->addAction (::resIcon ("delete"), tr ("Remove"));
          action->setData (RemovePlaylist);
        }

        if (!mdbItem->isContainer ())
        {
          QString           playlistName = mdbItem->playlistName ();
          CPlaylist const & playlist     = m_playlists.value (playlistName);
          if (!playlist.items ().isEmpty ())
          {
            menu->addSeparator ();
            action = menu->addAction (::resIcon ("search16"), tr ("Check playlist"));
            action->setData (CheckPlaylist);
          }
        }
      }
    }
  }
}

void CMainWindow::aboutToShowContextualActions ()
{
  QMenu*              menu = ui->m_contextualActions->menu ();
  EStackedWidgetIndex index = static_cast<EStackedWidgetIndex>(ui->m_stackedWidget->currentIndex ());
  switch (index)
  {
    case Home :
      addContextualActions (ui->m_myDevice, menu);
      break;

    case ContentDirectory :
      addContextualActions (ui->m_contentDirectory, menu);
      break;

    case Queue :
      addContextualActions (ui->m_queue, menu);
      break;

    case Playlist :
      addContextualActions (ui->m_playlistContent, menu);
      break;

    case Playing :
      addContextualActions (nullptr, menu);
      break;

    default:
      break;
  }
}

void CMainWindow::contextualAction (QAction* action)
{
  if (action != nullptr)
  {
    EActions type = static_cast<EActions>(action->data ().toInt ());
    if (type == Search)
    {
      searchAction (ui->m_provider->isReadOnly ());
    }
    else
    {
      searchAction (false);
      QWidget* widget = ui->m_stackedWidget->currentWidget ();
      if (widget == ui->m_pContentDirectory || widget == ui->m_pQueue || widget == ui->m_pPlaylistContent)
      {
        QListWidget*            lw            = widget->findChild<QListWidget*> ();
        QList<QListWidgetItem*> selectedItems = lw->selectedItems ();
        if (!selectedItems.isEmpty ())
        {
          QString providerText = ui->m_provider->text ();
          switch (type)
          {
            case ReplaceQueue :
              ui->m_queue->clear ();
            case AddQueue :
            {
              QApplication::setOverrideCursor (Qt::WaitCursor);
              QList<CDidlItem> didlItems = this->didlItems (selectedItems);
              ui->m_queue->addItems (didlItems);
              QApplication::restoreOverrideCursor ();
              break;
            }

            case AddPlaylist :
            {
              QString          provider = ui->m_provider->text ();
              QList<CDidlItem> didlItems = this->didlItems (selectedItems);
              CPlaylist::EType type      = CPlaylist::playlistType (didlItems);

              QString title = ui->m_myDevice->containerItem (static_cast<CMyDeviceBrowserItem::EType>(type))->text ();
              CMyDeviceBrowserItem::removeCount (title);
              CPlaylistChoice plc (m_playlists, title, type, didlItems.size (), this);
              int ret = plc.exec ();
              if (ret == QDialog::Accepted)
              {
                QString    name        = plc.name ();
                bool       newPlaylist = !m_playlists.contains (name);
                CPlaylist& playlist    = m_playlists[name];
                playlist.setType (type);
                playlist.addItems (didlItems);
                if (newPlaylist)
                {
                  CMyDeviceBrowserItem* item = ui->m_myDevice->addItem (type, name);
                  item                       = ui->m_myDevice->containerItem (item);
                  if (item != nullptr && ui->m_myDevice->isCollapsed (item))
                  {
                    ui->m_myDevice->setCollapsed (item, false);
                  }
                }
              }

              ui->m_provider->setText (provider);
              break;
            }

            case AddFavorites :
            {
              QList<CDidlItem> didlItems = this->didlItems (selectedItems);
              CPlaylist::EType type      = CPlaylist::favoritesType (didlItems);
              if (type != CPlaylist::Unknown)
              {
                for (QMap<QString, CPlaylist>::iterator it = m_playlists.begin (), end = m_playlists.end (); it != end; ++it)
                {
                  CPlaylist& playlist = it.value ();
                  if (playlist.type () == type)
                  {
                    playlist.addItems (didlItems);
                    break;
                  }
                }
              }

              break;
            }

            case RemoveTracks :
              if (lw == ui->m_playlistContent || lw == ui->m_queue)
              {
                static_cast<CPlaylistBrowser*>(lw)->delKey ();
                ui->m_contextualActions->setEnabled (lw->count () != 0);
              }
              break;

            default :
              break;
          }

          ui->m_provider->setText (providerText);
        }
      }
      else if (widget == ui->m_pHome || widget == ui->m_pDebug)
      {
        switch (type)
        {
          case ScanNetwork :
            m_cp->avDiscover ();
            break;

          case RemovePlaylist :
            ui->m_myDevice->delKey ();
            break;

          case RenamePlaylist :
            ui->m_myDevice->rename ();
            break;

          case CheckPlaylist :
          {
            QList<QListWidgetItem*> items = ui->m_myDevice->selectedItems (); // The selected playlist
            if (!items.isEmpty ())
            {
              ui->m_playlistContent->blockIconUpdate (true);
              CMyDeviceBrowserItem* item = static_cast<CMyDeviceBrowserItem*>(items.first ()); // The playlist
              on_m_myDevice_itemDoubleClicked (item); // Go to playlist content

              QString           playlistName = item->playlistName ();
              CPlaylist&        playlist     = m_playlists[playlistName];
              QList<CDidlItem>& didlItems    = playlist.items ();
              int               cDidlItems   = didlItems.size ();
              if (cDidlItems != 0)
              {
                CContentDirectory cd (m_cp);
                QList<int>        indices; // Invalid items indices.
                indices.reserve (cDidlItems);

                // Initialize the progress dialog.
                QProgressDialog progress (playlistName, tr ("Abort"), 0, cDidlItems, this);
                removeWindowContextHelpButton (&progress);
                progress.setWindowTitle (tr ("Check playlist..."));
                progress.setWindowModality (Qt::WindowModal);

                for (int iItem = 0; iItem < cDidlItems; ++iItem)
                {
                  progress.setValue (iItem);
                  QColor color;
                  if (!cd.isValidItem (didlItems[iItem])) // Check validity.
                  { // Store invalid item index.
                    indices.push_back (iItem);
                    color = Qt::red;
                  }
                  else
                  {
                    color = Qt::darkGreen;
                  }

                  QListWidgetItem* iItemItem = ui->m_playlistContent->item (iItem);
                  if (iItemItem != nullptr)
                  {
                    iItemItem->setForeground (color);
                    ui->m_playlistContent->scrollToItem (iItemItem);
                  }

                  if (progress.wasCanceled ())
                  {
                    indices.clear ();
                    break;
                  }
                }

                if (!indices.isEmpty ())
                { // Try to update item from title.
                  QMap<QString, bool> servers;
                  CServerScanner scanner (m_cp, didlItems[indices.first ()].type (), &progress); // Initialize the forlder scanner.

                  int cIndices = indices.size (), cUpdated = 0;

                  // Initialize progress dialog for update.
                  progress.setWindowTitle (tr ("Update playlist..."));
                  QProgressBar* pb       = progress.findChild<QProgressBar*> ();
                  pb->setMaximum (cIndices);

                  QString root ("0");
                  QString criteria ("dc:title contains \"%1\"");
                  for (int index = 0; index < cIndices; ++index)
                  {
                    progress.setValue (index);
                    CDidlItem& didlItem    = didlItems[indices[index]];
                    QString    serverUUID  = m_cp->deviceUUID (didlItem.uri (0), CDevice::MediaServer); // Get the server from uri ip address.
                    if (m_status.hasStatus (UseSearchForCheckPlaylist) && !servers.contains (serverUUID))
                    {
                      QStringList searchCaps = cd.getSearchCaps (serverUUID);
                      bool        dcTitle    = std::find (searchCaps.cbegin (), searchCaps.cend (), "dc:title") != searchCaps.cend ();
                      servers.insert (serverUUID, dcTitle);
                    }

                    CDidlItem  newDidlItem;
                    bool       dcTitle = servers.value (serverUUID);
                    if (dcTitle)
                    {
                      QString      titleCriteria = criteria.replace ("%1", didlItem.title ());
                      CBrowseReply reply         = cd.search (serverUUID, root, titleCriteria);
                      if (reply.numberReturned () > 0)
                      {
                        newDidlItem = reply.items ().first ();
                      }
                    }
                    else
                    {
                      newDidlItem = scanner.scan (serverUUID, didlItem.title ()); // Launch server scan
                    }

                    if (!newDidlItem.isEmpty ())
                    { // Found same title.
                      CContentDirectoryBrowserItem* cdbItem = static_cast<CContentDirectoryBrowserItem*>(ui->m_playlistContent->item (indices[index]));
                      cdbItem->setForeground (Qt::darkGreen);
                      cdbItem->resetIcon ();
                      didlItem = newDidlItem; // Replace item in the playlist.
                      ui->m_playlistContent->scrollToItem (cdbItem);
                      ++cUpdated;
                    }

                    QString text = tr ("Updates: ") + QString::number (cUpdated) +
                                   tr (" Fails: ") + QString::number (cIndices - cUpdated);
                    progress.setLabelText (text);
                    if (progress.wasCanceled ())
                    {
                      break;
                    }
                  }

                  progress.setValue (cIndices);
                  if (cUpdated)
                  {
                    playlist.setChanged ();
                  }
                }
                else
                {
                  progress.setValue (cDidlItems);
                }

                ui->m_playlistContent->blockIconUpdate (false);
                ui->m_playlistContent->startIconUpdateTimer ();
              }
            }
            break;
          }

          default :
            break;
        }
      }
    }
  }
}

void CMainWindow::addHomePopupMenu ()
{
  QMenu* menu = ui->m_home->menu ();
  if (menu == nullptr)
  {
    menu = new QMenu (this);
    ui->m_home->setMenu (menu);
    ui->m_home->setPopupMode (QToolButton::InstantPopup);
    connect (menu, &QMenu::triggered, this, &CMainWindow::homeAction);

    QAction* action = menu->addAction (::resIcon ("settings"),  QString ());
    action->setData (Settings);

    action = menu->addAction (::resIcon ("language"),  QString ());
    action->setData (Language);

    action = menu->addAction (::resIcon ("about"),  QString ());
    action->setData (About);
  }

  QList<QAction*> actions = menu->actions ();
  QString         texts[] = { tr ("Settings"), tr ("Language"), tr ("About") };
  for (int iAction = 0, cActions = actions.size (); iAction < cActions; ++iAction)
  {
    actions[iAction]->setText (texts[iAction]);
  }
}

void CMainWindow::homeAction (QAction* action)
{
  EActions type = static_cast<EActions>(action->data ().toInt ());
  switch (type)
  {
    case Settings :
    {
      EStatus status[] = { ShowNetworkCom,
                           UseSearchForCheckPlaylist,
                           UPnPEventsOnly,
                           UPnPPlaylistDisabled,
                           ShowCloudServers,
                         };

      bool flags[CSettings::LastIndex];
      for (int index = CSettings::ShowNetworkCom; index < CSettings::LastIndex; ++index)
      {
        flags[index] = m_status.hasStatus (status[index]);
      }

      QSize     iconSize = m_iconSize;
      CSettings settings (flags, m_iconSize, this);
      if (settings.exec () == QDialog::Accepted)
      {
        for (int index = CSettings::ShowNetworkCom; index < CSettings::LastIndex; ++index)
        {
          if (flags[index])
          {
            m_status.addStatus (status[index]);
          }
          else
          {
            m_status.remStatus (status[index]);
          }
        }

        ui->m_queue->setDisableUPnPPlaylist (m_status.hasStatus (UPnPPlaylistDisabled));
        ui->m_networkProgress->setHidden (!m_status.hasStatus (ShowNetworkCom));
      }

      if (settings.resetState ())
      {
        resize (m_startSize);
      }

      if (m_iconSize != iconSize)
      {
        CContentDirectoryBrowser* cds[] = { ui->m_contentDirectory, ui->m_queue, ui->m_playlistContent };
        m_pixmapCache->clear ();
        for (CContentDirectoryBrowser* cd : cds)
        {
          cd->setIconType (CContentDirectoryBrowserItem::IconNotdefined);
        }

        cds[0]->startIconUpdateTimer ();
      }

      break;
    }

    case About :
    {
      CAbout about (this);
      about.exec ();
      break;
    }

    case Language :
    {
      CLanguage language (this);
      int ret = language.exec ();
      if (ret == QDialog::Accepted)
      {
        QString lang = language.shortNameSelected ();
        if (lang != m_language)
        {
          QString qmFile = language.qmFileSelected ();
          if (m_translator != nullptr)
          {
            QCoreApplication::removeTranslator (m_translator);
            delete m_translator;
            m_translator = nullptr;
          }

          m_language = lang;
          if (!qmFile.isEmpty ())
          {
            QString qmPath = appFileDir () + "/languages/" + qmFile;
            m_translator   = new QTranslator;
            m_translator->load (qmPath);
            QCoreApplication::installTranslator (m_translator);
          }
          else
          {
            QCoreApplication::removeTranslator (m_translator);
            delete m_translator;
            m_translator = nullptr;
          }
        }
      }
      break;
    }

    default :
      break;
  }
}

void CMainWindow::contextMenuRequested (QPoint const & pos)
{
  QListWidget* lw = static_cast<QListWidget*>(sender ());
  QMenu        menu;
  addContextualActions (lw, &menu);

  CContentDirectoryBrowser::stopIconUpdateTimer ();
  QPoint   globalPos      = lw->viewport ()->mapToGlobal (pos);
  QAction* selectedAction = menu.exec (globalPos);
  if (selectedAction != nullptr)
  {
    contextualAction (selectedAction);
  }
}
