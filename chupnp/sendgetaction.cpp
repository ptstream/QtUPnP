#include "mainwindow.hpp"
#include "helper.hpp"
#include "ui_mainwindow.h"
#include "../upnp/avcontrol.hpp"

#include <QRegularExpression>

static void addArg (QTreeWidgetItem* item, QString const & name, int value)
{
  new QTreeWidgetItem (item, QStringList (name + '=' + QString::number (value)));
}

static void addArg (QTreeWidgetItem* item, QString const & name, unsigned value)
{
  new QTreeWidgetItem (item, QStringList (name + '=' + QString::number (value)));
}

static void addArg (QTreeWidgetItem* item, QString const & name, bool value)
{
  new QTreeWidgetItem (item, QStringList (name + '=' + (value ? "true" : "false")));
}

static void addArg (QTreeWidgetItem* item, QString const & name, QString const & value)
{
  new QTreeWidgetItem (item, QStringList (name + '=' + value));
}

static void addArg (QTreeWidgetItem* item, QString const & name, QStringList const & values)
{
  item = new QTreeWidgetItem (item, QStringList (name));
  for (QString const & value : values)
  {
    new QTreeWidgetItem (item, QStringList (value));
  }
}

void CMainWindow::sendGetAction (QTreeWidgetItem* item)
{
  if (item == nullptr)
  {
    return;
  }

  clearError (item);
  removeActionArgs (item);
  QString name = item->text (0);
  name.remove (QRegularExpression (" \\(\\d+ms\\)"));
  if (name == "GetMute")
  {
    bool state = m_ctl.renderingControl ().getMute (m_deviceUUID); // Get mute state.
    addArg (item, "Mute", state);
    applyError (item);
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_mute = state;
  }
  else if (name == "GetVolume")
  {
    int volume = m_ctl.renderingControl ().getVolume (m_deviceUUID); // Get volume.
    addArg (item, "Volume", volume);
    applyError (item);
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_vol = volume;
  }
  else if (name == "ListPresets")
  {
    m_current.m_presets = m_ctl.renderingControl ().getListPresets (m_deviceUUID);
    addArg (item, "Presets", m_current.m_presets);
    applyError (item);
    addElapsedTime (item);
    setGetActionItemColor (item);
  }
  else if (name == "GetCurrentConnectionIDs")
  {
    QStringList replies = m_ctl.connectionManager ().getCurrentConnectionIDs (m_deviceUUID); // Get connection IDs. Must be "0".
    applyError (item);
    addArg (item, "Current connection ID", replies);
    addElapsedTime (item);
    setGetActionItemColor (item);
  }
  else if (name.startsWith ("GetProtocolInfo"))
  {
    QVector<QStringList> replies = m_ctl.connectionManager ().getProtocolInfos (m_deviceUUID); // Get the mime types supported by the renderer.
    applyError (item);
    if (replies.size () > 1)
    {
      addArg (item, "Source", replies[0]);
      addArg (item, "Sink", replies[1]);
    }

    addElapsedTime (item);
    setGetActionItemColor (item);
  }
  else if (name == "GetCurrentTransportActions")
  {
    QStringList replies = m_ctl.avTransport ().getCurrentTransportActions (m_deviceUUID); // Get the valid current actions.
    applyError (item);
    addArg (item, "Actions", replies);
    addElapsedTime (item);
    setGetActionItemColor (item);
  }
  else if (name == "GetMediaInfo")
  {
    CMediaInfo info = m_ctl.avTransport ().getMediaInfo (m_deviceUUID); // Get the media info.
    applyError (item);
    addArg (item, "Nr tracks", info.nrTracks ());

    QString const & duration = info.mediaDuration ();
    m_current.m_duration.clear ();
    if (timeMs (duration) != 0)
    {
      m_current.m_duration = duration;
    }

    addArg (item, "Duration", duration);

    m_current.m_avTransportURI = info.currentURI ();
    addArg (item, "Current uri", m_current.m_avTransportURI);
    addArg (item, "Current uri metadata", info.currentURIMetaData ());

    m_current.m_nextAVTransportURI = info.nextURI ();
    addArg (item, "Next uri", m_current.m_nextAVTransportURI);
    addArg (item, "Next uri metadata", info.nextURIMetaData ());

    addArg (item, "Play Medium", info.playMedium ());
    addArg (item, "Record Medium", info.recordMedium ());
    addArg (item, "Write status", info.writeStatus ());
    addElapsedTime (item);
    setGetActionItemColor (item);
  }
  else if (name == "GetPositionInfo")
  {
    CPositionInfo info = m_ctl.avTransport ().getPositionInfo (m_deviceUUID); // Get the position info for the current transport.
    applyError (item);
    addArg (item, "Track number", info.track ());
    addArg (item, "Track duration", info.trackDuration ());
    addArg (item, "Track metadata", info.trackMetaData());
    addArg (item, "Track uri", info.trackURI ());
    addArg (item, "Track relative time", info.relTime ());
    addArg (item, "Track absolute time", info.absTime ());
    addArg (item, "Track relative count", info.relCount ());
    addArg (item, "Track absolute count", info.absCount ());

    addElapsedTime (item);
    setGetActionItemColor (item);
    if (timeMs (info.trackDuration ()) != 0)
    {
      m_current.m_duration = info.trackDuration ();
    }

    if (timeMs (info.relTime ()) != 0)
    {
      m_current.m_relTime = info.relTime ();
    }
  }
  else if (name == "GetTransportInfo")
  {
    // Get the transport infos PAUSE_PLAYBACK, STOP, PLAY...
    CTransportInfo info = m_ctl.avTransport ().getTransportInfo (m_deviceUUID);
    applyError (item);
    addArg (item, "Current transport state", info.currentTransportState ());
    addArg (item, "Current transport status", info.currentTransportStatus ());
    addArg (item, "Current speed", info.currentSpeed ());
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_transportInfo = info.currentTransportState ();
  }
  else if (name == "GetTransportSettings")
  {
    // Get the travsport settings (NORMAL, SUFFLE...).
    CTransportSettings settings = m_ctl.avTransport ().getTransportSettings (m_deviceUUID);
    applyError (item);
    addArg (item, "PlayMode", settings.playMode ());
    addArg (item, "RecPlayMode", settings.recQualityMode ());
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_transportSettings = settings.playMode ();
  }
  else if (name == "GetDeviceCapabilities")
  {
    CDeviceCaps caps = m_ctl.avTransport ().getDeviceCaps (m_deviceUUID);
    applyError (item);
    addArg (item, "Play medias", caps.playMedias ());
    addArg (item, "Record medias", caps.recMedias ());
    addArg (item, "Record quality modes", caps.recQualityModes ());
    addElapsedTime (item);
    setGetActionItemColor (item);
  }
  else if (name == "GetSearchCapabilities")
  {
    m_current.m_searchCaps = m_ctl.contentDirectory ().getSearchCaps (m_deviceUUID);
    applyError (item);
    addArg (item, "Capabilities", m_current.m_searchCaps);
    addElapsedTime (item);
    setGetActionItemColor (item);
  }
  else if (name == "GetSortCapabilities")
  {
    QStringList caps = m_ctl.contentDirectory ().getSortCaps (m_deviceUUID);
    applyError (item);
    addArg (item, "Capabilities", caps);
    addElapsedTime (item);
    setGetActionItemColor (item);
  }
  else if (name == "GetSystemUpdateID")
  {
    unsigned id = m_ctl.contentDirectory ().getSystemUpdateID (m_deviceUUID);
    applyError (item);
    addArg (item, "ID", id);
    addElapsedTime (item);
    setGetActionItemColor (item);
  }
  else if (name == "GetCurrentConnectionInfo")
  {
    CConnectionInfo info = m_ctl.connectionManager ().getCurrentConnectionInfo (m_deviceUUID);
    applyError (item);
    addArg (item, "RcsID", info.rcsID ());
    addArg (item, "AVTransportID", info.avTransportID ());
    addArg (item, "ProtocolInfo", info.protocolInfo ());
    addArg (item, "PeerConnectionManager", info.peerConnectionManager ());
    addArg (item, "PeerConnectionID", info.peerConnectionID ());
    addArg (item, "Direction", info.direction ());
    addArg (item, "Status", info.status ());
    addElapsedTime (item);
    setGetActionItemColor (item);
  }
  else if (name == "GetBrightness")
  {
    unsigned value = m_ctl.renderingControl ().getBrightness (m_deviceUUID);
    addArg (item, "Brightness", value);
    applyError (item);
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_brightness = value;
  }
  else if (name == "GetContrast")
  {
    int value = m_ctl.renderingControl ().getContrast (m_deviceUUID);
    addArg (item, "Contrast", value);
    applyError (item);
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_contrast = value;
  }
  else if (name == "GetSharpness")
  {
    int value = m_ctl.renderingControl ().getSharpness (m_deviceUUID);
    addArg (item, "Sharpness", value);
    applyError (item);
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_sharpness = value;
  }
  else if (name == "GetRedVideoGain")
  {
    int value = m_ctl.renderingControl ().getRedVideoGain (m_deviceUUID);
    addArg (item, "RedVideoGain", value);
    applyError (item);
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_redVideoGain = value;
  }
  else if (name == "GetRedVideoBlackLevel")
  {
    int value = m_ctl.renderingControl ().getRedVideoBlackLevel (m_deviceUUID);
    addArg (item, "RedVideoBlackLevel", value);
    applyError (item);
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_redVideoBlackLevel = value;
  }
  else if (name == "GetGreenVideoGain")
  {
    int value = m_ctl.renderingControl ().getGreenVideoGain (m_deviceUUID);
    addArg (item, "GreenVideoGain", value);
    applyError (item);
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_greenVideoGain = value;
  }
  else if (name == "GetGreenVideoBlackLevel")
  {
    int value = m_ctl.renderingControl ().getGreenVideoBlackLevel (m_deviceUUID);
    addArg (item, "GreenVideoBlackLevel", value);
    applyError (item);
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_greenVideoBlackLevel = value;
  }
  else if (name == "GetBlueVideoGain")
  {
    int value = m_ctl.renderingControl ().getBlueVideoGain (m_deviceUUID);
    addArg (item, "BlueVideoGain", value);
    applyError (item);
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_blueVideoGain = value;
  }
  else if (name == "GetBlueVideoBlackLevel")
  {
    int value = m_ctl.renderingControl ().getBlueVideoBlackLevel (m_deviceUUID);
    addArg (item, "BlueVideoBlackLevel", value);
    applyError (item);
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_blueVideoBlackLevel = value;
  }
  else if (name == "GetColorTemperature")
  {
    int value = m_ctl.renderingControl ().getColorTemperature (m_deviceUUID);
    addArg (item, "ColorTemperature", value);
    applyError (item);
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_colorTemperature = value;
  }
  else if (name == "GetHorizontalKeystone")
  {
    int value = m_ctl.renderingControl ().getHorizontalKeystone (m_deviceUUID);
    addArg (item, "HorizontalKeystone", value);
    applyError (item);
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_horizontalKeystone = value;
  }
  else if (name == "GetVerticalKeystone")
  {
    int value = m_ctl.renderingControl ().getVerticalKeystone (m_deviceUUID);
    addArg (item, "VerticalKeystone", value);
    applyError (item);
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_verticalKeystone = value;
  }
  else if (name == "GetVolumeDB")
  {
    int value = m_ctl.renderingControl ().getVolumeDB (m_deviceUUID);
    addArg (item, "VolumeDB", value);
    applyError (item);
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_volDB = value;
  }
  else if (name == "GetVolumeDBRange")
  {
    QPair<int, int> minmax = m_ctl.renderingControl ().getVolumeDBRange (m_deviceUUID);
    addArg (item, "Min", minmax.first);
    addArg (item, "Max", minmax.second);
    applyError (item);
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_volMinDB = minmax.first;
    m_current.m_volMaxDB = minmax.second;
  }
  else if (name == "GetLoudness")
  {
    bool state = m_ctl.renderingControl ().getLoudness (m_deviceUUID); // Get mute state.
    addArg (item, "loudness", state);
    applyError (item);
    addElapsedTime (item);
    setGetActionItemColor (item);
    m_current.m_loudness = state;
  }
}

void CMainWindow::updateGetAction (QString const & name)
{
  QStringList const & actions = m_actionLinks.value (name);
  for (QString const & action : actions)
  {
    QTreeWidgetItem* item = findActionItem (action);
    if (item != nullptr)
    {
      sendGetAction (item);
      item->setExpanded (true);
    }
  }
}
