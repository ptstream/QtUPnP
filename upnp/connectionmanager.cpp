#include "connectionmanager.hpp"
#include "controlpoint.hpp"
#include "actioninfo.hpp"

USING_UPNP_NAMESPACE

QStringList CConnectionManager::getCurrentConnectionIDs (QString const & deviceUUID)
{
  Q_ASSERT (m_cp != nullptr);
  QStringList ids;
  QList<CControlPoint::TArgValue> args;
  args << CControlPoint::TArgValue ("ConnectionIDs",  QString ());
  CActionInfo actionInfo = m_cp->invokeAction (deviceUUID, "GetCurrentConnectionIDs", args);
  if (actionInfo.succeeded ())
  {
    ids = fromCVS (args.last ().second);
    if (!ids.isEmpty () && ids.first () != "0")
    {
      CDevice& device = m_cp->device (deviceUUID);
      device.setConnectionID (ids.first ().toUInt ());
    }
  }

  return ids;
}

QVector<QStringList> CConnectionManager::getProtocolInfos (QString const & deviceUUID)
{
  Q_ASSERT (m_cp != nullptr);
  QVector<QStringList> protocols (2);
  QList<CControlPoint::TArgValue> args;
  args.reserve (2);
  args << CControlPoint::TArgValue ("Source",  QString ());
  args << CControlPoint::TArgValue ("Sink",  QString ());
  CActionInfo actionInfo = m_cp->invokeAction (deviceUUID, "GetProtocolInfo", args);
  if (actionInfo.succeeded ())
  {
    protocols[Source] = fromCVS (args[0].second);
    protocols[Sink]   = fromCVS (args[1].second);
  }

  return protocols;
}

CConnectionInfo CConnectionManager::getCurrentConnectionInfo (QString const & deviceUUID)
{
  Q_ASSERT (m_cp != nullptr);
  CConnectionInfo connectionInfo;
  CDevice const & device = m_cp->device (deviceUUID);

  QString                         id = QString::number (device.connectionID ());
  QList<CControlPoint::TArgValue> args;
  args.reserve (8);
  args << CControlPoint::TArgValue ("ConnectionID", id);
  args << CControlPoint::TArgValue ("RcsID",  QString ());
  args << CControlPoint::TArgValue ("AVTransportID",  QString ());
  args << CControlPoint::TArgValue ("ProtocolInfo",  QString ());
  args << CControlPoint::TArgValue ("PeerConnectionManager",  QString ());
  args << CControlPoint::TArgValue ("PeerConnectionID",  QString ());
  args << CControlPoint::TArgValue ("Direction",  QString ());
  args << CControlPoint::TArgValue ("Status",  QString ());

  CActionInfo actionInfo = m_cp->invokeAction (deviceUUID, "GetCurrentConnectionInfo", args);
  if (actionInfo.succeeded ())
  {
    connectionInfo.setRcsID (args[1].second.toInt ());
    connectionInfo.setAVTransportID (args[2].second.toInt ());
    connectionInfo.setProtocolInfo (args[3].second);
    connectionInfo.setPeerConnectionManager (args[4].second);
    connectionInfo.setPeerConnectionID (args[5].second.toInt ());
    connectionInfo.setDirection (args[6].second);
    connectionInfo.setStatus (args[7].second);
  }

  return connectionInfo;
}
