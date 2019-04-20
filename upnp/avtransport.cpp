#include "avtransport.hpp"
#include "controlpoint.hpp"
#include "actioninfo.hpp"
#include "../upnp/waitingloop.hpp"
#include "../upnp/plugin.hpp"

USING_UPNP_NAMESPACE

bool CAVTransport::waitForAVTransportURI (QString const & renderer, int iTrack)
{
  int           cChecks = 0;
  CPositionInfo info;
  do
  {
    CWaitingLoop::wait (500, QEventLoop::ExcludeUserInputEvents);
    info = getPositionInfo (renderer);
    ++cChecks;
  }
  while (cChecks < 5 && static_cast<int>(info.track ()) != iTrack);
  CWaitingLoop::wait (500, QEventLoop::ExcludeUserInputEvents); // Caution.
  return cChecks < 5;
}

QStringList CAVTransport::getCurrentTransportActions (QString const & rendererUUID, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  QStringList                     actions;
  QList<CControlPoint::TArgValue> args;
  args.reserve (2);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue ("Actions",  QString ());
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, "GetCurrentTransportActions", args);
  if (actionInfo.succeeded ())
  {
    actions = fromCVS (args.last ().second);
  }

  return actions;
}

CTransportInfo CAVTransport::getTransportInfo (QString const & rendererUUID, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  CTransportInfo                  transportInfo;
  QList<CControlPoint::TArgValue> args;
  args.reserve (4);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue ("CurrentTransportState",  QString ());
  args << CControlPoint::TArgValue ("CurrentTransportStatus",  QString ());
  args << CControlPoint::TArgValue ("CurrentSpeed",  QString ());
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, "GetTransportInfo", args);
  if (actionInfo.succeeded ())
  {
    transportInfo.setCurrentTransportState (args[1].second);
    transportInfo.setCurrentTransportStatus (args[2].second);
    transportInfo.setCurrentSpeed (args[3].second);
  }

  return transportInfo;
}

CTransportSettings CAVTransport::getTransportSettings (QString const & rendererUUID, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  CTransportSettings              settings;
  QList<CControlPoint::TArgValue> args;
  args.reserve (3);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue ("PlayMode",  QString ());
  args << CControlPoint::TArgValue ("RecQualityMode",  QString ());
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, "GetTransportSettings", args);
  if (actionInfo.succeeded ())
  {
    settings.setPlayMode (args[1].second);
    settings.setRecQualityMode (args[2].second);
  }

  return settings;
}

CDeviceCaps CAVTransport::getDeviceCaps (QString const & rendererUUID, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  CDeviceCaps                     caps;
  QList<CControlPoint::TArgValue> args;
  args.reserve (4);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue ("PlayMedia",  QString ());
  args << CControlPoint::TArgValue ("RecMedia",  QString ());
  args << CControlPoint::TArgValue ("RecQualityModes",  QString ());
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, "GetDeviceCapabilities", args);
  if (actionInfo.succeeded ())
  {
    caps.setPlayMedias (fromCVS (args[1].second));
    caps.setRecMedias (fromCVS (args[2].second));
    caps.setRecQualityModes (fromCVS (args[3].second));
  }

  return caps;
}

CPositionInfo CAVTransport::getPositionInfo (QString const & rendererUUID, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  CPositionInfo                   positionInfo;
  QList<CControlPoint::TArgValue> args;
  args.reserve (9);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue ("Track",  QString ());
  args << CControlPoint::TArgValue ("TrackDuration",  QString ());
  args << CControlPoint::TArgValue ("TrackMetaData",  QString ());
  args << CControlPoint::TArgValue ("TrackURI",  QString ());
  args << CControlPoint::TArgValue ("RelTime",  QString ());
  args << CControlPoint::TArgValue ("AbsTime",  QString ());
  args << CControlPoint::TArgValue ("RelCount",  QString ());
  args << CControlPoint::TArgValue ("AbsCount",  QString ());
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, "GetPositionInfo", args);
  if (actionInfo.succeeded ())
  {
    positionInfo.setTrack (args[1].second.toUInt ());
    positionInfo.setTrackDuration (args[2].second);
    positionInfo.setTrackMetaData (args[3].second);
    positionInfo.setTrackURI (args[4].second);
    positionInfo.setRelTime (args[5].second);
    positionInfo.setAbsTime (args[6].second);
    positionInfo.setRelCount (args[7].second.toInt ());
    positionInfo.setAbsCount (args[8].second.toInt ());
  }

  return positionInfo;
}

CMediaInfo CAVTransport::getMediaInfo (QString const & rendererUUID, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  CMediaInfo                      mediaInfo;
  QList<CControlPoint::TArgValue> args;
  args.reserve (10);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue ("NrTracks",  QString ());
  args << CControlPoint::TArgValue ("MediaDuration",  QString ());
  args << CControlPoint::TArgValue ("CurrentURI",  QString ());
  args << CControlPoint::TArgValue ("CurrentURIMetaData",  QString ());
  args << CControlPoint::TArgValue ("NextURI",  QString ());
  args << CControlPoint::TArgValue ("NextURIMetaData",  QString ());
  args << CControlPoint::TArgValue ("PlayMedium",  QString ());
  args << CControlPoint::TArgValue ("RecordMedium",  QString ());
  args << CControlPoint::TArgValue ("WriteStatus",  QString ());
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, "GetMediaInfo", args);
  if (actionInfo.succeeded ())
  {
    mediaInfo.setNrTracks (args[1].second.toUInt ());
    mediaInfo.setMediaDuration (args[2].second);
    mediaInfo.setCurrentURI (args[3].second);
    mediaInfo.setCurrentURIMetaData (args[4].second);
    mediaInfo.setNextURI (args[5].second);
    mediaInfo.setNextURIMetaData (args[6].second);
    mediaInfo.setPlayMedium (args[7].second);
    mediaInfo.setRecordMedium (args[8].second);
    mediaInfo.setWriteStatus (args[9].second);
  }

  return mediaInfo;
}

// Single file
bool CAVTransport::setAVTransportURI (QString const & rendererUUID, CDidlItem const & item,
                                      int index, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  m_cp->abortStreaming ();
  CActionInfo actionInfo;
  QString uri = item.uri (index);
  if (!uri.isEmpty ())
  {
    uri = replace127_0_0_1 (uri); // WMP problem.
    QList<CControlPoint::TArgValue> args;
    args.reserve (3);
    args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
    args << CControlPoint::TArgValue ("CurrentURI", uri);
    args << CControlPoint::TArgValue ("CurrentURIMetaData",item.didl ());
    actionInfo = m_cp->invokeAction (rendererUUID, "SetAVTransportURI", args);
    if (!actionInfo.succeeded () && !args[2].second.isEmpty ())
    { // The server returned an error, try withou metadata.
      args[2].second.clear ();
      actionInfo = m_cp->invokeAction (rendererUUID, "SetAVTransportURI", args);
    }
  }

  return actionInfo.succeeded ();
}

// Playlists
bool CAVTransport::setAVTransportURI (QString const & rendererUUID, QString const & playlistName,
                                      QList<CDidlItem::TPlaylistElem> const & items,
                                      CDidlItem::EPlaylistFormat format, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  m_cp->abortStreaming ();
  bool        success = false;
  CActionInfo actionInfo;
  if (m_cp->setPlaylistContent (items, format) != 0)
  {
    QString uri = m_cp->playlistURI (playlistName);
    QList<CControlPoint::TArgValue> args;
    args.reserve (3);
    args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
    args << CControlPoint::TArgValue ("CurrentURI", uri);
    args << CControlPoint::TArgValue ("CurrentURIMetaData",  QString ());
    actionInfo = m_cp->invokeAction (rendererUUID, "SetAVTransportURI", args);
    success    = actionInfo.succeeded ();
    if (success)
    {
      m_cp->setPlaylistName (playlistName);
    }
  }

  return success;
}

bool CAVTransport::setAVTransportURI (QString const & rendererUUID, QString const & uri, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  m_cp->abortStreaming ();
  CActionInfo actionInfo;
  if (!uri.isEmpty ())
  {
    QList<CControlPoint::TArgValue> args;
    args.reserve (3);
    args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
    args << CControlPoint::TArgValue ("CurrentURI", replace127_0_0_1 (uri));
    args << CControlPoint::TArgValue ("CurrentURIMetaData",  QString ());
    actionInfo = m_cp->invokeAction (rendererUUID, "SetAVTransportURI", args);
  }

  return actionInfo.succeeded ();
}

bool CAVTransport::setNextAVTransportURI (QString const & rendererUUID, CDidlItem const & item,
                                          int index, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  CActionInfo actionInfo;
  QString     uri = item.uri (index);
  if (!uri.isEmpty ())
  {
    QList<CControlPoint::TArgValue> args;
    args.reserve (3);
    args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
    args << CControlPoint::TArgValue ("NextURI", uri);
    args << CControlPoint::TArgValue ("NextURIMetaData", item.didl ());
    actionInfo = m_cp->invokeAction (rendererUUID, "SetNextAVTransportURI", args);
  }

  return actionInfo.succeeded ();
}

bool CAVTransport::setNextAVTransportURI (QString const & rendererUUID, QString const & uri, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  CActionInfo actionInfo;
  if (!uri.isEmpty ())
  {
    QList<CControlPoint::TArgValue> args;
    args.reserve (3);
    args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
    args << CControlPoint::TArgValue ("NextURI", uri);
    args << CControlPoint::TArgValue ("NextURIMetaData",  QString ());
    actionInfo = m_cp->invokeAction (rendererUUID, "SetNextAVTransportURI", args);
  }

  return actionInfo.succeeded ();
}

bool CAVTransport::setPlayMode (QString const & rendererUUID, QString const & mode, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  QList<CControlPoint::TArgValue> args;
  args.reserve (2);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue ("NewPlayMode", mode);
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, "SetPlayMode", args);
  return actionInfo.succeeded ();
}

bool CAVTransport::setPPS (QString const & rendererUUID, QString const & actionName, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  QList<CControlPoint::TArgValue> args;
  args.reserve (2);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  if (actionName == "Play")
  {
    args << CControlPoint::TArgValue ("Speed", "1");
  }
  else if (actionName == "Stop")
  {
    m_cp->abortStreaming ();
  }

  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, actionName, args);
  return actionInfo.succeeded ();
}

bool CAVTransport::play (QString const & rendererUUID, unsigned instanceID)
{
  return setPPS (rendererUUID, "Play", instanceID);
}

bool CAVTransport::pause (QString const & rendererUUID, unsigned instanceID)
{
  return setPPS (rendererUUID, "Pause", instanceID);
}

bool CAVTransport::stop (QString const & rendererUUID, unsigned instanceID)
{
  return setPPS (rendererUUID, "Stop", instanceID);
}

bool CAVTransport::next (QString const & rendererUUID, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  m_cp->abortStreaming ();
  QList<CControlPoint::TArgValue> args;
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, "Next", args);
  return actionInfo.succeeded ();
}

bool CAVTransport::previous (QString const & rendererUUID, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  m_cp->abortStreaming ();
  QList<CControlPoint::TArgValue> args;
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, "Previous", args);
  return actionInfo.succeeded ();
}

bool CAVTransport::seek (QString const & rendererUUID, QString const & timePosition, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  QString time = timePosition;
  if (time.isEmpty () || time == "Beginning")
  {
    time = "00:00:00";
  }

  QList<CControlPoint::TArgValue> args;
  args.reserve (3);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue ("Unit", "REL_TIME");
  args << CControlPoint::TArgValue ("Target", time);
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, "Seek", args);
  return actionInfo.succeeded ();
}

bool CAVTransport::seek (QString const & rendererUUID, int iTrack, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  m_cp->abortStreaming ();
  QList<CControlPoint::TArgValue> args;
  args.reserve (3);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue ("Unit", "TRACK_NR");
  args << CControlPoint::TArgValue ("Target", QString::number (iTrack));
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, "Seek", args);
  return actionInfo.succeeded ();
}
