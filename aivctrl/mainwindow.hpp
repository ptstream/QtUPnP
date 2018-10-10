#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "playlist.hpp"
#include "../upnp/device.hpp"
#include "../upnp/status.hpp"
#include <QMainWindow>
#include <QNetworkReply>
#include <QTimer>

namespace Ui
{
  class CMainWindow;
}

namespace QtUPnP
{
  class CControlPoint;
  class CPixmapCache;
  class CDidlItem;
}

class CContentDirectoryBrowserItem;
class CFolderItem;
class CNetworkProgress;

class QListWidget;
class QListWidgetItem;
class QTranslator;

/*! The QMainWindow. */
class CMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  /*! Flags. */
  enum EStatus { ShowNetworkCom            = 0x00000001, //!< Hide or show the network communications widget.
                 UseSearchForCheckPlaylist = 0x00000002, //!< Use search action to check playlists.
                 UPnPEventsOnly            = 0x00000004, //!< Use UPnP events only when changing of renderer.
                 CreateDumpErrorFile       = 0x00000008, //!< Generate dump error file.
                 CollapseAudioPlaylist     = 0x00000010, //!< Collapse or expang audio playlist.
                 CollapseImagePlaylist     = 0x00000020, //!< Collapse or expang image playlist.
                 CollapseVideoPlaylist     = 0x00000040, //!< Collapse or expang video playlist.
                 UPnPPlaylistDisabled      = 0x00000080, //!< The UPnP playlists are disable. Use SetAVTransportURI.
               };

  /*! Stacked widget index. */
  enum EStackedWidgetIndex { Home,
                             ContentDirectory,
                             Queue,
                             Playlist,
                             Playing,
                             Dump,
                           };

  /*! Play mode. */
  enum EPlayMode { Normal,
                   RepeatAll,
                   RepeatOne,
                   Shuffle,
                   Random,
                   LastPlayMode,
                 };

  /*! Contextual menu actions. */
  enum EActions { Search,
                  ReplaceQueue,
                  AddQueue,
                  AddPlaylist,
                  AddFavorites,
                  RemoveTracks,
                  RemovePlaylist,
                  RenamePlaylist,
                  CheckPlaylist,
                  ScanNetwork,
                  Settings,
                  About,
                  Language,
                };

  /*! Playlists. */
  typedef QMap<QString, CPlaylist> TMPlaylists;

  /*! Constructor. */
  explicit CMainWindow(QWidget *parent = 0);

  /*! Destructor. */
  ~CMainWindow();

protected slots :
  // QtUPnP
  /*! A new device is detected. */
  void newDevice (QString const & uuid);

  /*! A device is closed. */
  void lostDevice (QString const & uuid);

  /*! An UPnP event is emitted. */
  void eventReady (QStringList const & emitter);

  /*! An UPnP error is detected. */
  void upnpError (int error, QString const & description);

  /*! An network error is detected. */
  void networkError (QString const & device, QNetworkReply::NetworkError error, QString const & description);

  // ListWidgets
  /*! Server QListWidget item clicked. */
  void on_m_servers_itemClicked (QListWidgetItem* item);
  void on_m_cloud_itemClicked (QListWidgetItem* item);

  /*! Server content directory QListWidget item double clicked. */
  void on_m_contentDirectory_itemDoubleClicked (QListWidgetItem* item);

  /*! playlist content QListWidget item double clicked. */
  void on_m_playlistContent_itemDoubleClicked (QListWidgetItem* item);

  /*! Queue QListWidget item double clicked. */
  void on_m_queue_itemDoubleClicked (QListWidgetItem* item);

  /*! My device QListWidget item clicked. */
  void on_m_myDevice_itemClicked (QListWidgetItem* item);

  /*! My device QListWidget item double clicked. */
  void on_m_myDevice_itemDoubleClicked (QListWidgetItem* item);

  /*! A new playlist must be created. */
  void newPlaylist (QString const & name, int type);

  /*! A playlist must be renamed. */
  void renamePlaylist (QString const & name, QString const & newName);

  /*! A playlist must be removed. */
  void removePlaylist (QString const & name);

  /*! A playlist item order must be changed. */
  void rowsMoved (QModelIndex const &, int start, int end, QModelIndex const &, int row);

  /*! Playlist items must be remove. */
  void removeIDs (QString const & name, QStringList const & ids);

  //Buttons
  /*! The previous stacked widget button has been clicked. */
  void on_m_previousStackedWidgetIndex_clicked ();

  /*! The home button has been clicked. */
  void on_m_home_clicked ();

  /*! The current queue playing has been clicked to see the current playing or the current queue. */
  void on_m_currentQueuePlaying_clicked ();

  /*! The play-pause button has been clicked. */
  void on_m_play_clicked ();

  /*! The play-pause button has been clicked. */
  void on_m_play2_clicked ();

  /*! The mute button has been clicked. */
  void on_m_mute_clicked ();

  /*! The mute button has been clicked. */
  void on_m_mute2_clicked ();

  /*! The pevious button has been clicked. */
  void on_m_previous_clicked ();

  /*! The pevious button has been clicked. */
  void on_m_previous2_clicked ();

  /*! The next button has been clicked. */
  void on_m_next_clicked ();

  /*! The next button has been clicked. */
  void on_m_next2_clicked ();

  /*! The absTime button has been clicked (toggle to show remainig time or track time. */
  void on_m_absTime_clicked ();

  /*! The current playing has been clicked. */
  void on_m_currentPlayingIcon_clicked ();

  /*! The repeat button has been clicked. */
  void on_m_repeat_clicked ();

  /*! The shuffle button has been clicked. */
  void on_m_shuffle_clicked ();

  /*! The favorite button has been clicked. */
  void on_m_favorite_clicked ();

  // StackedWidget
  /*! The current stack index has changed. */
  void on_m_stackedWidget_currentChanged (int index);

  // Menu and actions
  /*! The renderers menu will be displayed. */
  void aboutToShowRenderer ();

  /*! An action of the renderers menu has been triggered. */
  void rendererAction (QAction* action);

  /*! The contextual menu will be displayed. */
  void aboutToShowContextualActions ();

  /*! An action of the contextual menu has been triggered. */
  void contextualAction (QAction* action);

  /*! An action of the home menu has been triggered. */
  void homeAction (QAction* action);

  /*! A right hand menu must be displayed. */
  void contextMenuRequested (QPoint const & pos);

  // Slider
  /*! The volume must be changed. */
  void on_m_volume_valueChanged (int volume);

  /*! The volume must be changed. */
  void on_m_volume2_valueChanged (int volume);

  /*! The volume must matched the cursor position . */
  void on_m_volume_actionTriggered (int action);

  /*! The volume must matched the cursor position . */
  void on_m_volume2_actionTriggered (int action);

  /*! The position (seek) must be changed. */
  void on_m_position_valueChanged (int position);

  /*! The position (seek) must matched the cursor position. */
  void on_m_position_actionTriggered (int action);

  // QLineEdit
  /*! Launch search at each charater. */
  void on_m_provider_textChanged (QString const & text);

  // Others
  /*! Change the server folder when m_folder QLineEdit hans been clicked. */
  void changeFolder (int index);

  /*! Update the position slider (once at seconde). */
  void updatePosition ();

  /*! Show a dump message. */
  void showDump (); // Activate by CTRL+F11 keyboard keys.

  /*! Launch or staop search action for CTRL+F. */
  void search ();

  /*! A network communication has started. */
  void networkComStarted (QtUPnP::CDevice::EType type);

  /*! A network communication has ended. */
  void networkComEnded (QtUPnP::CDevice::EType type);

protected :
  /*! Discovery timer event. */
  virtual void timerEvent (QTimerEvent* event);

  /*! Main window close event. */
  virtual void closeEvent (QCloseEvent* event);

  /*! Change event. Used for translation. */
  virtual void changeEvent (QEvent* event);

private :
  // ListWidget icons.
  /*! Returns the servers icon list. Use for default icon. */
  QStringList serverIcons ();

  /*! Returns the content directory icon list. Use for default icon. */
  QStringList contentDirectoryIcons ();

  /*! Sets the icon size for all QListWidget. */
  void setIconSize ();

  /*! Update the content directory widget. */
  void updateContentDirectory (CFolderItem const & item, bool parentID);

  // Popup menu manager.
  /*! Adds contextual actions at a menu. */
  void addContextualActions (QListWidget* lw, QMenu* menu);

  /*! Adds renderer popupmenu at renderer button. */
  void addRendererPopupMenu ();

  /*! Adds home popupmenu at home button. */
  void addHomePopupMenu ();

  // Update widgets.
  /*! Initializes some widgets. */
  void initWidgets ();

  /*! Sets the mute icon */
  void muteIcon (bool mute);

  /*! Sets the playing icon. */
  void playingIcon (bool playing);

  /*! Updates the volume slider when the current renderer change. */
  void updateVolumeSlider (int volume, bool blocked = false);

  /*! Updates the current playing.
   * \param didl: The CDidlItem use to update widgets.
   * \param startPlaying: True to start the update current playlig timer.
   * \param fullUpdate: False for UPnP event.
   */
  void updateCurrentPlayling (QtUPnP::CDidlItem const & didl, bool startPlaying, bool fullUpdate);

  /*! Updates the current playing.
   * \param item: The CContentDirectoryBrowserItem use to update widgets.
   * \param startPlaying: True to start the position timer.
   * \param fullUpdate: False for UPnP event.
   */
  void updateCurrentPlayling (CContentDirectoryBrowserItem const * item, bool startPlaying, bool fullUpdate);

  /*! Clears the current playing. */
  void clearCurrentPlayling ();

  /*! Starts the position timer. */
  void startPositionTimer ();

  /*! Stops the position timer. */
  void stopPositionTimer ();

  /*! Renderer stopped. Update the buttons */
  void rendererStopped ();

  /*! Toggles the position timer. */
  void togglePositionTimer (bool playing);

  /*! Sets Bold the item of QListWidgets with the uri equal at uri. */
  void setItemBold (QString const & uri);

  /*! Sets Bold the item of QListWidget. */
  void setItemBold (CContentDirectoryBrowserItem const * item);

  /*! Sets Bold the item of QListWidgets equal at item. */
  void setItemBold (QtUPnP::CDidlItem const & item);

  /*! Parses play mode for the current renderer. */
  QString playModeString ();

  /*! returns tye play mode from play mode string. */
  EPlayMode playMode (QString const & currentPlayModeString);

  /*! Apply the last session at start. */
  void applyLastSession ();

  /*! Applys the current play mode. */
  void applyPlayMode ();

  /*! Rotates the red settings icon. */
  void rotateIcon ();

  /*! Returns the CDidlItems list from a server CDidlItem. This function invokes the brows action. */
  int didlItems (QList<QtUPnP::CDidlItem>& didlItems, QtUPnP::CDidlItem const & item);

  /*! Returns the CDidlItems list from a server QListWidgetItem list. */
  QList<QtUPnP::CDidlItem> didlItems (QList<QListWidgetItem*> lwItems);

  /*! The queue has changed, From the bold item update the queue and the position. */
  void currentQueueChanged ();

  /*! Updates the play lists item count. */
  void updatePlaylistItemCount ();

  /*! Update the device count during discovery. */
  void updateDevicesCount (int *cServers = nullptr, int *cRenderers = nullptr);

  /*! Enable or disable search. */
  void searchAction (bool activate);

  /*! The current server has changed, update the current communication server icon. */
  void setComServerIcon ();

  /*! Creates plugin items in ui->m_cloud. */
  void loadPlugins ();

  /*! The current renderer has changed, update the current communication renderer icon. */
  void setComRendererIcon ();

  // Actions
  /*! Sets a new transport from a QListWidget item. */
  void setAVTransport (CContentDirectoryBrowserItem const * item);

  /*! Sets the next or previous transport. */
  bool nextItem (bool forward);

  /*! Network communications. */
  CNetworkProgress* networkProgress (QtUPnP::CDevice::EType type);

private : // UPnP
  Ui::CMainWindow* ui = nullptr; // The ui of the main windows.
  QtUPnP::CControlPoint* m_cp = nullptr; // The control point.
  int m_idDicoveryTimer = -1; // The discovery timer ID.
  int m_iconAngle = 0; // The current angle of the red settings icon.
  QtUPnP::CPixmapCache* m_pixmapCache = nullptr; // The pixmap cache.
  QString m_server; //!< Current server. // The current server.
  QString m_renderer; //!< Current renderer. // The current renderer.
  QtUPnP::CStatus m_status; // The status of the application.
  QTimer m_positionTimer; // The position timer.
  int m_positionTimerInterval = 1000; //!< 1s. The position timer interval.
  int m_discoverWaitInterval = 1000; //!< 1s. The wait interval during dicovery.
  int m_cDiscoverWaits = 0; // The current number of discovery retries.
  int m_cDiscoverMaxWaits = 20; // The max number of discovery retries.
  bool m_discoveryGuard = false; // The guard has been started.
  bool m_iconRotated = true; // The current state of the red settings icon.
  EPlayMode m_playMode = Normal; // Current playing mode.
  TMPlaylists m_playlists; // The playlists.
  QTranslator* m_translator = nullptr; // The current translator.
  QString m_language; // The current language.
  QString m_providerText; // The curent text of provider QLineEdit (use to store and restore).

private : // UI
  QSize m_iconSize = QSize (32, 32); // Icon size of the application.
};

#endif // MAINWINDOW_HPP
