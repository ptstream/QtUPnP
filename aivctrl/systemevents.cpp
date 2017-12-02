#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "session.hpp"
#include "aivwidgets/widgethelper.hpp"
#include "../upnp/avtransport.hpp"
#include <QMenu>

USING_UPNP_NAMESPACE

void CMainWindow::timerEvent (QTimerEvent* event)
{
  auto updateActions = [this] () -> QList<QAction*>
  {
    aboutToShowRenderer ();
    QMenu* menu = ui->m_renderer->menu ();
    return menu->actions ();
  };

  if (m_idDicoveryTimer == event->timerId ())
  {
    killTimer (m_idDicoveryTimer);
    if (m_cDiscoverWaits == 0)
    { // Launch discovery
      ui->m_home->setIcon (::resIcon ("discovery0"));
      m_iconAngle = 0;
      m_cp->avDiscover ();
    }

    if (m_cDiscoverWaits == m_cDiscoverMaxWaits - 1)
    { // Not find the last session renderer. Try to find a renderer playing.
      m_renderer.clear ();
    }

    QString rendererLastSession = m_renderer;
    if (m_cp->hasRenderer () && ui->m_servers->count () != 0 && m_cDiscoverWaits < m_cDiscoverMaxWaits)
    { // Try to find the last session renderer.
      if (!m_renderer.isEmpty ())
      {
        m_renderer.clear ();
        QList<QAction*> actions = updateActions ();
        for (QAction* action : actions)
        {
          QString renderer = action->data ().toString ();
          if (renderer == rendererLastSession)
          {
            rendererAction (action);
            m_cDiscoverWaits = m_cDiscoverMaxWaits;
          }
        }
      }
      else
      { // Try to find a renderer playing.
        QList<QAction*> actions = updateActions ();
        if (!actions.isEmpty ())
        {
          CAVTransport avt (m_cp);
          for (QAction* action : actions)
          {
            QString        renderer      = action->data ().toString ();
            CTransportInfo transportInfo = avt.getTransportInfo (renderer);
            if (transportInfo.currentTransportState () == "PLAYING")
            {
              rendererAction (action);
              m_cDiscoverWaits = m_cDiscoverMaxWaits;
            }
          }
        }
      }
    }

    if (m_cDiscoverWaits < m_cDiscoverMaxWaits)
    { // Find nothing wait 1s more.
      m_renderer        = rendererLastSession;
      m_idDicoveryTimer = startTimer (m_discoverWaitInterval);
      ++m_cDiscoverWaits;
      if (m_iconRotated)
      {
        rotateIcon ();
      }
    }
    else
    {
      if (!m_discoveryGuard)
      {
        if (m_renderer.isEmpty ())
        { // Find nothing select the first renderer
          QStringList uuids = m_cp->renderers ();
          if (!uuids.isEmpty ())
          {
            QList<QAction*> actions = updateActions ();
            if (!actions.isEmpty ())
            {
              rendererAction (actions.first ());
            }
          }
        }

        m_discoveryGuard = true;
        m_idDicoveryTimer = startTimer (10000); // More 10s
        if (m_iconRotated)
        {
          rotateIcon ();
        }
      }
    }
  }
}

void CMainWindow::closeEvent (QCloseEvent* event)
{
  searchAction (false);
  savePlaylists (m_playlists);
  CContentDirectoryBrowser::stopIconUpdateTimer ();
  m_positionTimer.stop ();
  if (m_cp != nullptr)
  {
    CSession session (m_renderer, ui->m_volume->value (), m_playMode,
                      ui->m_absTime->isChecked (), normalGeometry (), windowState (),
                      m_language, m_status.status ());
    session.save ();
    m_cp->close ();
  }

  QMainWindow::closeEvent (event);
}

void CMainWindow::changeEvent (QEvent* event)
{
  if (event->type () == QEvent::LanguageChange)
  {
    ui->retranslateUi (this); // retranslate designer forms
    ui->m_myDevice->retranslateSpecialItems ();
    updatePlaylistItemCount ();
    addHomePopupMenu ();
    if (ui->m_playlistContent->count () != 0)
    {
      ui->m_provider->setText (ui->m_myDeviceLabel->text ());
    }
    else if (!ui->m_myDevice->selectedItems ().isEmpty ())
    {
      ui->m_provider->setText (ui->m_serverLabel->text ());
    }
  }

  QWidget::changeEvent (event); // remember to call base class implementation
}
