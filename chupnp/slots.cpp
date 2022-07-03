#include "mainwindow.hpp"
#include "actionParams.hpp"
#include "stdaction.hpp"
#include "item.hpp"
#include "helper.hpp"
#include "ui_mainwindow.h"
#include <QTreeWidget>
#include <QProgressBar>
#include <QClipboard>
#include <QElapsedTimer>
#include <QRegularExpression>

void CMainWindow::newDevice (QString const & uuid)
{
  CDevice const & device = m_cp->device (uuid);
  if (!device.isSubDevice ())
  {
    CDevice::EType   type = device.type ();
    int              parentIndex; // Top level category.
    QTreeWidgetItem* parentItem;
    switch (type)
    {
      case CDevice::MediaServer :
        parentIndex = 0; // It's a server.
        break;

      case CDevice::MediaRenderer :
        parentIndex = 1; // It's a renderer.
        break;

      default :
        parentIndex = 2; // It's an other type.
        break;
    }

    parentItem = ui->m_devices->topLevelItem (parentIndex); // Get parent item.
    insertItem (parentItem, device);
    parentItem->sortChildren (0, Qt::AscendingOrder);

    int     childCount = parentItem->childCount ();
    QString text       = parentItem->text (0);
    int     index      = text.lastIndexOf (' ');
    if (index != -1)
    { // Remove current number.
      text.truncate (index);
    }

    text += QString (" (%2)").arg (childCount); // Add the new number.
    parentItem->setText (0, text);
    parentItem->setExpanded (true);
  }
}

void CMainWindow::lostDevice (QString const & uuid)
{
  QTreeWidget* trees[] = { ui->m_devices, ui->m_services, ui->m_events };
  for (QTreeWidget* tree : trees)
  {
    QList<QTreeWidgetItem*> items = tree->findItems (uuid, Qt::MatchContains | Qt::MatchRecursive, 0);
    for (QTreeWidgetItem* item : items)
    {
      delete item;
    }
  }
}

void CMainWindow::eventReady (QStringList const & emitter)
{
  if (emitter.size () >= 2)
  {
    // device uuid at position 0.
    // Service identifier at position 1.
    CDevice const &  device = m_cp->device (emitter[0]); // Device uuid.

    // The top level item text has the form of : time->name [uuid] [service].
    QString text   = QTime::currentTime ().toString () + "->" +
                     device.name () + " [" + emitter[1] + "] [" +
                     emitter[0] + ']';
    QTreeWidgetItem* item = new QTreeWidgetItem (ui->m_events, QStringList (text)); // Create the event item.

    for (QStringList::const_iterator it = emitter.cbegin () + 2, end = emitter.cend (); it != end; ++it)
    { // For each state variable.
      QString const & varName = *it; // Get the state variable name.
      CStateVariable  var     = device.stateVariable (varName, emitter[1]); // Get the state variable from its name and the service.
      text                    = varName + ':';
      text                   += var.type () == CStateVariable::String ?
                                '"' + var.toString () + '"' : var.toString ();// Add value
      QTreeWidgetItem* itemValue = new QTreeWidgetItem (item, QStringList (text)); // Create the state variable item.
      QList<TConstraint> const & csts = var.constraints (); // Get the constraints of the variable.
      if (!csts.isEmpty ())
      {
        for (TConstraint const & cst : csts)
        {
          text = cst.first + ':' + cst.second; // Constraint item text has the form : name:value
          new QTreeWidgetItem (itemValue, QStringList (text)); // Create the constraint item
        }
      }
    }

    ui->m_events->scrollToBottom ();
  }
}

void CMainWindow::upnpError (int errorCode, QString const & errorString)
{
  QString text = QString ("Error:%1 (%2)").arg (errorCode).arg (errorString);
  statusBar ()->showMessage (text);
}

void CMainWindow::networkError (QString const & deviceUUID, QNetworkReply::NetworkError errorCode, QString const & errorString)
{
  QString name = m_cp->device (deviceUUID).name ();
  QString text = QString ("Device:%1, Error:%2 (%3)").arg (name).arg (errorCode).arg (errorString);
  statusBar ()->showMessage (text);
}

void CMainWindow::discoveryLaunched (char const * nt, int index, int count)
{
  statusBar ()->showMessage (nt);
  m_pb->setMaximum (count);
  m_pb->setValue (index);
}

void CMainWindow::on_m_devices_itemDoubleClicked (QTreeWidgetItem *item, int column)
{
  if (item->parent () != nullptr)
  {
    m_current.reset ();
    ui->m_tabWidget->setCurrentIndex (1);
    QString uuid = item->data (column, Qt::UserRole).toString ();
    ui->m_events->clear ();

    QElapsedTimer ti;
    ti.start ();
    loadServices (uuid);
    if (subscription (uuid))
    {
      setItemColor (item, QColor (Qt::blue));
    }

    addTotalTime (item, ti.elapsed ());
  }
}

void CMainWindow::on_m_services_itemDoubleClicked (QTreeWidgetItem* item, int column)
{
  statusBar ()->clearMessage ();
  int type = item->type ();
  if (type == ActionType)
  {
    QString actionName = item->text (column);
    actionName.remove (QRegularExpression (" \\(\\d+ms\\)"));
    if (actionName == "GetMute" ||
        actionName == "GetVolume"  ||
        actionName == "GetMediaInfo" ||
        actionName == "GetTransportInfo" ||
        actionName == "GetTransportSettings" ||
        actionName == "GetMediaInfo" ||
        actionName == "GetPositionInfo" ||
        actionName == "GetCurrentTransportActions" ||
        actionName == "GetSearchCapabilities" ||
        actionName == "GetSortCapabilities" ||
        actionName == "GetCurrentConnectionIDs" ||
        actionName == "GetSystemUpdateID" ||
        actionName == "GetProtocolInfo" ||
        actionName == "GetCurrentConnectionInfo" ||
        actionName == "GetDeviceCapabilities" ||
        actionName == "ListPresets"
        )
    {
      clearError (item);
      sendGetAction (item);
      applyError (item);
      item->setExpanded (true);
    }
    else if (actionName == "SetMute")
    {
      if (CActionParams::sendMute (m_current.m_mute))
      {
        clearError (item);
        m_ctl.renderingControl ().setMute (m_deviceUUID, m_current.m_mute);
        applyError (item);
        addElapsedTime (item);
        updateGetAction (actionName);
      }
    }
    else if (actionName == "SetVolume")
    {
      if (CActionParams::sendVolume (m_current.m_vol, m_current.m_volMin, m_current.m_volMax, m_current.m_volStep))
      {
        clearError (item);
        m_ctl.renderingControl ().setVolume (m_deviceUUID, m_current.m_vol);
        applyError (item);
        addElapsedTime (item);
        updateGetAction (actionName);
      }
    }
    else if (actionName == "SetVolumeDB")
    {
      if (CActionParams::sendVolumeDB (m_current.m_volDB, m_current.m_volMinDB, m_current.m_volMaxDB))
      {
        clearError (item);
        m_ctl.renderingControl ().setVolumeDB (m_deviceUUID, m_current.m_volDB);
        applyError (item);
        addElapsedTime (item);
        updateGetAction (actionName);
      }
    }
    else if (actionName == "SetAVTransportURI")
    {
      clearError (item);
      if (!m_selectedTracks.isEmpty ())
      {
        m_current.m_avTransportURI = m_cp->playlistURI ("playlist");
      }

      if (CActionParams::sendAVTransportURI (m_current.m_avTransportURI))
      {
        if (!m_selectedTracks.isEmpty ())
        {
          QList<CDidlItem::TPlaylistElem> elems;
          elems.reserve (m_selectedTracks.size ());
          for (CDidlItem const & item : m_selectedTracks)
          {
            elems.push_back (CDidlItem::TPlaylistElem (item, 0));
          }

          m_ctl.avTransport ().setAVTransportURI (m_deviceUUID, "playlist", elems, CDidlItem::M3u);
        }
        else
        {
          m_ctl.avTransport ().setAVTransportURI (m_deviceUUID, m_current.m_avTransportURI);
        }

        applyError (item);
        addElapsedTime (item);
        updateGetAction (actionName);
      }
    }
    else if (actionName == "SetNextAVTransportURI")
    {
      clearError (item);
      if (CActionParams::sendNextAVTransportURI (m_current.m_nextAVTransportURI))
      {
        m_ctl.avTransport ().setNextAVTransportURI (m_deviceUUID, m_current.m_nextAVTransportURI);
        applyError (item);
        addElapsedTime (item);
        updateGetAction ("SetAVTransportURI");
      }
    }
    else if (actionName == "Pause" || actionName == "Play" || actionName == "Stop")
    {
      if (CActionParams::sendPPS (m_current.m_transportInfo, actionName))
      {
        clearError (item);
        m_ctl.avTransport ().setPPS (m_deviceUUID, actionName);
        applyError (item);
        addElapsedTime (item);
        updateGetAction ("PPS");
      }
    }
    else if (actionName == "Previous" || actionName == "Next")
    {
      if (CActionParams::sendPN (actionName))
      {
        clearError (item);
        if (actionName == "Next")
        {
          m_ctl.avTransport ().next (m_deviceUUID);
        }
        else
        {
          m_ctl.avTransport ().previous (m_deviceUUID);
        }

        applyError (item);
        addElapsedTime (item);
        updateGetAction ("PN");
      }
    }
    else if (actionName == "SetPlayMode")
    {
      if (CActionParams::sendPlayMode (m_current.m_transportSettings, m_current.m_playModes))
      {
        clearError (item);
        m_ctl.avTransport ().setPlayMode (m_deviceUUID, m_current.m_transportSettings);
        applyError (item);
        addElapsedTime (item);
        updateGetAction (actionName);
      }
    }
    else if (actionName == "Seek")
    {
      if (CActionParams::sendSeek (m_current.m_relTime, m_current.m_duration))
      {
        clearError (item);
        m_ctl.avTransport ().seek (m_deviceUUID, m_current.m_relTime);
        applyError (item);
        addElapsedTime (item);
        updateGetAction (actionName);
      }
    }
    else if (actionName == "Browse")
    {
      browse (item);
    }
    else if (actionName == "SelectPreset")
    {
      if (CActionParams::selectPreset (m_current.m_preset, m_current.m_presets))
      {
        clearError (item);
        m_ctl.renderingControl ().selectPreset (m_deviceUUID, m_current.m_preset);
        applyError (item);
        addElapsedTime (item);
        updateGetAction (actionName);
      }
    }
    else
    {
      CStdAction sac (m_cp, m_deviceUUID, actionName);
      sac.exec ();
    }
  }
  else if (type == BrowseType)
  {
    browse (item);
  }
  else if (type == SearchType)
  {
    if (CActionParams::sendSearch (m_current.m_criteria, m_current.m_searchCaps))
    {
      search (item, m_current.m_criteria);
    }
  }
}

void CMainWindow::on_m_services_itemClicked (QTreeWidgetItem* item, int)
{
  switch (item->type ())
  {
    case BrowseType :
    {
      CItem*      didlItem  = static_cast<CItem*>(item);
      QString     uri       = didlItem->didl ().uri (0);
      QClipboard* clipboard = QApplication::clipboard ();
      clipboard->setText (uri);
      m_selectedTracks.clear ();
      break;
    }
  }
}

void CMainWindow::timeout ()
{
  m_timer.stop ();
  m_initRunning = false;
  restoreDevices ();
  on_m_rescan_triggered ();
}
