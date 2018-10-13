#include "mainwindow.hpp"
#include "helper.hpp"
#include "ui_mainwindow.h"
#include "aivwidgets/widgethelper.hpp"
#include "../upnp/xmlhdidllite.hpp"
#include <QMenu>

USING_UPNP_NAMESPACE

void CMainWindow::newDevice (QString const & uuid)
{
  CDevice const & device = m_cp->device (uuid);
  if (!device.isSubDevice ())
  {
    CDevice::EType type = device.type ();
    if (type == CDevice::MediaServer)
    {
      ui->m_servers->addItem (device);
    }
  }

  int cServers, cRenderers;
  updateDevicesCount (&cServers, &cRenderers);
  if (m_iconRotated && cServers > 0 && cRenderers > 0)
  {
    m_iconRotated = false;
    ui->m_home->setIcon (::resIcon ("home"));
  }
}

void CMainWindow::lostDevice (QString const & uuid)
{
  ui->m_servers->delItem (uuid); // If uuid is not a server, it does not matter because uuid is unique.
  if (uuid == m_server)
  {
    m_server.clear ();
    setComServerIcon ();
    ui->m_contentDirectory->clear ();
    ui->m_queue->clear ();
    clearCurrentPlayling ();
    ui->m_stackedWidget->setCurrentIndex (Home);
  }
  else
  {
    QMenu*          menu    = ui->m_renderer->menu ();
    QList<QAction*> actions = menu->actions ();
    for (QAction* action : actions)
    {
      if (action->data ().toString () == uuid)
      {
        actions.removeOne (action);
        delete action;
        break;
      }
    }

    if (uuid == m_renderer)
    {
      stopPositionTimer ();
      m_renderer.clear ();
      if (!actions.isEmpty ())
      {
        rendererAction (actions.first ());
      }
      else
      {
        clearCurrentPlayling ();
        ui->m_rendererName->clear ();
        ui->m_renderer->setIcon (::resIcon ("renderer"));
        setComRendererIcon ();
      }
    }
  }

  updateDevicesCount ();
}

//#define DUMPEVENT  qDebug () << name << ':' << variant;

void CMainWindow::eventReady (QStringList const & emitter)
{
  if (!emitter.isEmpty ())
  {
    QString const & device  = emitter[0];
    QString const & service = emitter[1];
    CStateVariable  variable;
    QVariant        variant;
    for (QStringList::const_iterator it = emitter.cbegin () + 2, end = emitter.cend (); it != end; ++it)
    {
      QString const & name = *it;
      variable             = m_cp->stateVariable (device, service, name);
      if (name == "Mute" || name == "Volume" || name == "VolumeDB")
      {
        QList<TConstraint> constraint;
        constraint << TConstraint ("channel", "master");
        variant = variable.value (constraint);
        if (!variant.isValid ())
        {
          variant = variable.value ();
        }
      }
      else
      {
        variant = variable.value ();
      }

      if (variant.isValid ())
      {
#ifdef DUMPEVENT
        if (name != "LastChange")
        {
          DUMPEVENT
        }
#endif
        if (name == "Volume")
        {
          int volume = variant.toInt ();
          updateVolumeSlider (volume, true);
        }
        else if (name == "Mute")
        {
          muteIcon (variant.toBool ());
        }
        else if (name == "TransportState")
        {
          QString state      = variant.toString ();
          bool    playing    = state == "PLAYING" || state == "TRANSITIONING";
          bool    startTimer = playing;
          if (!playing)
          {
            CDevice const & device = m_cp->device (m_renderer);
            if ((ui->m_queue->isUPnPPlaylistDisabled () ||
                 device.playlistStatus () != CDevice::PlaylistHandler) &&
                !device.hasAction (QString::null, "setNextTransportURI"))
            {
              startTimer = true;
            }
          }

          // Asume PLAYING follow TRANSITIONING because some renderers send only TRANSITIONING.
          playingIcon (playing);
          togglePositionTimer (startTimer);
        }
        else if (name == "CurrentPlayMode")
        {
          m_playMode = playMode (variant.toString ());
          applyPlayMode ();
        }
        else if (name == "CurrentTrackDuration")
        {
          ui->m_position->blockSignals (true);
          ui->m_position->setMaximum (timeToS (variant.toString ()));
          ui->m_position->blockSignals (false);
        }
        else if (/*name == "AVTransportURI" || */name == "CurrentTrackURI")
        {
          QString uri = variant.toString ();
          setItemBold (uri);
          if (m_cp->playlistName ().isEmpty ())
          {
            ui->m_queue->setNextAVTransportURI (m_cp, m_renderer);
          }
        }
        else if (/*name == "AVTransportURIMetaData" || */name == "CurrentTrackMetaData")
        {
          QString       metadata = variant.toString ();
          CXmlHDidlLite h;
          CDidlItem     item = h.firstItem (metadata);
          updateCurrentPlayling (item, false, false);
        }
      }
    }
  }
}

void CMainWindow::upnpError (int error, QString const & description)
{
  if (m_status.hasStatus (CreateDumpErrorFile))
  {
    QString text = QString ("%1;UPnP error;%2%3")
                            .arg (QDateTime::currentDateTime ().toString ())
                            .arg (error).arg (description);
    dumpError (text);
  }
}

void CMainWindow::networkError (QString const & device, QNetworkReply::NetworkError error,
                                QString const & description)
{
  if (m_status.hasStatus (CreateDumpErrorFile))
  {
    QString deviceName = m_cp->device (device).name ();
    QString text       = QString ("%1;Network error;%2;%3;%4")
                                  .arg (QDateTime::currentDateTime ().toString ())
                                  .arg (deviceName).arg (error).arg (description);
    dumpError (text);
  }
}

