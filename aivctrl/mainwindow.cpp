#include "mainwindow.hpp"
#include "settings.hpp"
#include "helper.hpp"
#include "ui_mainwindow.h"
#include "aivwidgets/widgethelper.hpp"
#include "aivwidgets/networkprogress.hpp"
#include "../upnp/pixmapcache.hpp"
#include "../upnp/xmlh.hpp"
#include "../upnp/dump.hpp"
#include <QMenu>
#include <QShortcut>
#include <QDesktopWidget>

USING_UPNP_NAMESPACE

CMainWindow::CMainWindow (QWidget *parent) : QMainWindow (parent), ui (new Ui::CMainWindow)
{
  ui->setupUi (this);
  m_cp = new CControlPoint (this);
  connect (m_cp, SIGNAL(eventReady(QStringList const &)), this, SLOT(eventReady(QStringList const &)));
  connect (m_cp, SIGNAL(upnpError(int, QString const &)), this, SLOT(upnpError(int, QString const &)));
  connect (m_cp, SIGNAL(networkError(QString const &, QNetworkReply::NetworkError, QString const &)),
           this, SLOT(networkError(QString const &, QNetworkReply::NetworkError, QString const &)));
  connect (m_cp, SIGNAL(newDevice(QString const &)), this, SLOT(newDevice(QString const &)));
  connect (m_cp, SIGNAL(lostDevice(QString const &)), this, SLOT(lostDevice(QString const &)));

  connect (ui->m_folders, &CFolderBrowser::indexSelected, this, &CMainWindow::changeFolder);
  connect (&m_positionTimer, &QTimer::timeout, this, &CMainWindow::updatePosition);

  connect (ui->m_myDevice, &CMyDeviceBrowser::newPlaylist, this, &CMainWindow::newPlaylist);
  connect (ui->m_myDevice, &CMyDeviceBrowser::renamePlaylist, this, &CMainWindow::renamePlaylist);
  connect (ui->m_myDevice, &CMyDeviceBrowser::removePlaylist, this, &CMainWindow::removePlaylist);

  connect (ui->m_queue, &CPlaylistBrowser::rowsMoved, this, &CMainWindow::rowsMoved);
  connect (ui->m_queue, &CPlaylistBrowser::removeIDs, this, &CMainWindow::removeIDs);

  connect (ui->m_playlistContent, &CPlaylistBrowser::rowsMoved, this, &CMainWindow::rowsMoved);
  connect (ui->m_playlistContent, &CPlaylistBrowser::removeIDs, this, &CMainWindow::removeIDs);

  QKeySequence ks = QKeySequence(Qt::CTRL + Qt::Key_F11);
  connect (new QShortcut (ks, this), &QShortcut::activated, this, &CMainWindow::showDump);

  connect (new QShortcut (QKeySequence::Find, this), &QShortcut::activated, this, &CMainWindow::search);
  connect (new QShortcut (QKeySequence::MoveToPreviousChar, this), &QShortcut::activated, this, &CMainWindow::on_m_previous_clicked);
  connect (new QShortcut (QKeySequence::MoveToNextChar, this), &QShortcut::activated, this, &CMainWindow::on_m_next_clicked);

  connect (CDump::dumpObject (), SIGNAL(dumpReady(QString const &)), ui->m_dump, SLOT(insertPlainText(QString const &)));

  connect (m_cp, SIGNAL(networkComStarted(QtUPnP::CDevice::EType)), this, SLOT(networkComStarted(QtUPnP::CDevice::EType)));
  connect (m_cp, SIGNAL(networkComEnded(QtUPnP::CDevice::EType)), this, SLOT(networkComEnded(QtUPnP::CDevice::EType)));

  m_pixmapCache = new CPixmapCache;
  initWidgets ();
  ::removeDumpError ();
  if (m_status.hasStatus (CreateDumpErrorFile))
  {
    CXmlH::setDumpErrorFileName (errorFilePath ());
  }

  ui->m_myDevice->createDefaultPlaylists ();
  applyLastSession ();
  CSettings::setIconSize (this, m_iconSize);
  addRendererPopupMenu ();
  addHomePopupMenu ();
  ui->m_stackedWidget->setCurrentIndex (Home);
  ui->m_provider->setText (tr ("Start look for servers and renderers"));
  loadPlugins ();
  m_idDicoveryTimer = startTimer (500);
}

CMainWindow::~CMainWindow ()
{
  delete m_pixmapCache;
  delete ui;
}

void CMainWindow::initWidgets ()
{
  ui->m_folders->push (Home);

  QStringList              defIcons = contentDirectoryIcons ();
  CContentDirectoryBrowser* cds[]   = { ui->m_contentDirectory, ui->m_queue, ui->m_playlistContent };
  for (CContentDirectoryBrowser* cd : cds)
  {
    cd->setPixmapCache (m_pixmapCache);
    cd->setControlPoint (m_cp);
    cd->setDefaultIcons (defIcons);
    cd->setContextMenuPolicy (Qt::CustomContextMenu);
    connect (cd, &QWidget::customContextMenuRequested, this, &CMainWindow::contextMenuRequested);
  }

  ui->m_myDevice->setContextMenuPolicy (Qt::CustomContextMenu);
  connect (ui->m_myDevice, &QWidget::customContextMenuRequested, this, &CMainWindow::contextMenuRequested);

  setTransparentBackGround (ui->m_folders);
  setTransparentBackGround (ui->m_provider);
  setTransparentBackGround (ui->m_rendererName);
  setTransparentBackGround (ui->m_title);
  setTransparentBackGround (ui->m_trackMetadata);
  setTransparentBackGround (ui->m_cover);

  ui->m_folders->setReadOnly (true);
  ui->m_provider->setReadOnly (true);
  ui->m_rendererName->setReadOnly (true);
  ui->m_title->setReadOnly (true);

  defIcons = serverIcons ();
  ui->m_servers->setDefaultIcons (defIcons);
  QRect desktopRect    = QApplication::desktop ()->screenGeometry ();
  QRect mainWindowRect = geometry();
  if (mainWindowRect.height () > desktopRect.height ())
  {
    mainWindowRect.setHeight (desktopRect.height ());
    ui->m_cover->setMaximumHeight (128);
  }

  if (mainWindowRect.width () > desktopRect.width ())
  {
    mainWindowRect.setWidth (desktopRect.width ());
    ui->m_cover->setMaximumHeight (128);
  }

  ui->m_cover->setDefaultPixmapName ("audio_file_big");
  m_positionTimer.setInterval (m_positionTimerInterval);

  QMenu* menu = new QMenu (this);
  ui->m_contextualActions->setMenu (menu);
  connect (menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowContextualActions()));
  connect (menu, SIGNAL(triggered(QAction*)), this, SLOT(contextualAction(QAction*)));

#ifdef Q_OS_MACOS
  ui->m_volume->setTickPosition (QSlider::TicksBothSides);
  ui->m_volume2->setTickPosition (QSlider::TicksBothSides);
  ui->m_position->setTickPosition (QSlider::TicksBothSides);
#endif
}

