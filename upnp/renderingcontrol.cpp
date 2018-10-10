#include "renderingcontrol.hpp"
#include "controlpoint.hpp"
#include "actioninfo.hpp"

USING_UPNP_NAMESPACE

QStringList CRenderingControl::getListPresets (QString const & rendererUUID, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  QStringList                     presets;
  QList<CControlPoint::TArgValue> args;
  args.reserve (2);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue ("CurrentPresetNameList", QString::null);
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, "ListPresets", args);
  if (actionInfo.succeeded ())
  {
    presets = fromCVS (args.last ().second);
  }

  return presets;
}

bool CRenderingControl::getMute (QString const & rendererUUID, QString const & channel, unsigned instanceID)
{
  return getBoolChannel (rendererUUID, "GetMute", "CurrentMute", channel, instanceID);
}

int CRenderingControl::getVolume (QString const & rendererUUID, QString const & channel, unsigned instanceID)
{
  return getVolume (rendererUUID, "GetVolume", channel, instanceID);
}

bool CRenderingControl::selectPreset (QString const & rendererUUID, QString const & preset, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  m_cp->abortStreaming ();
  QList<CControlPoint::TArgValue> args;
  args.reserve (2);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue ("PresetName", preset);
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, "SelectPreset", args);
  return actionInfo.succeeded ();
}

bool CRenderingControl::setMute (QString const & rendererUUID, bool value, QString const & channel, unsigned instanceID)
{
  return setBoolChannel (rendererUUID, "SetMute", value, "DesiredMute", channel, instanceID);
}

bool CRenderingControl::setVolume (QString const & rendererUUID, unsigned value, QString const & channel, unsigned instanceID)
{
  return setVolume (rendererUUID, "SetVolume", value, channel, instanceID);
}

unsigned CRenderingControl::getBlueVideoBlackLevel (QString const & rendererUUID, unsigned instanceID)
{
  return getUInt1 (rendererUUID, "GetBlueVideoBlackLevel", "CurrentBlueVideoBlackLevel", instanceID);
}

unsigned CRenderingControl::getBlueVideoGain (QString const & rendererUUID, unsigned instanceID)
{
  return getUInt1 (rendererUUID, "GetBlueVideoGain", "CurrentBlueVideoGain", instanceID);
}

unsigned CRenderingControl::getBrightness (QString const & rendererUUID, unsigned instanceID)
{
  return getUInt1 (rendererUUID, "GetBrightness", "CurrentBrightness", instanceID);
}

unsigned CRenderingControl::getColorTemperature (QString const & rendererUUID, unsigned instanceID)
{
  return getUInt1 (rendererUUID, "GetColorTemperature", "CurrentColorTemperature", instanceID);
}

unsigned CRenderingControl::getContrast (QString const & rendererUUID, unsigned instanceID)
{
  return getUInt1 (rendererUUID, "GetContrast", "CurrentContrast", instanceID);
}

unsigned CRenderingControl::getGreenVideoBlackLevel (QString const & rendererUUID, unsigned instanceID)
{
  return getUInt1 (rendererUUID, "GetGreenVideoBlackLevel", "CurrentGreenVideoBlackLevel", instanceID);
}

unsigned CRenderingControl::getGreenVideoGain (QString const & rendererUUID, unsigned instanceID)
{
  return getUInt1 (rendererUUID, "GetGreenVideoGain", "CurrentGreenVideoGain", instanceID);
}

int CRenderingControl::getHorizontalKeystone (QString const & rendererUUID, unsigned instanceID)
{
  return getUInt1 (rendererUUID, "GetHorizontalKeystone", "CurrentHorizontalKeystone", instanceID);
}

bool CRenderingControl::getLoudness (QString const & rendererUUID, QString const & channel, unsigned instanceID)
{
  return getBoolChannel (rendererUUID, "GetLoudness", "CurrentLoudness", channel, instanceID);
}

unsigned CRenderingControl::getRedVideoBlackLevel (QString const & rendererUUID, unsigned instanceID)
{
  return getUInt1 (rendererUUID, "GetRedVideoBlackLevel", "CurrentRedVideoBlackLevel", instanceID);
}

unsigned CRenderingControl::getRedVideoGain (QString const & rendererUUID, unsigned instanceID)
{
  return getUInt1 (rendererUUID, "GetRedVideoGain", "CurrentRedVideoGain", instanceID);
}

unsigned CRenderingControl::getSharpness (QString const & rendererUUID, unsigned instanceID)
{
  return getUInt1 (rendererUUID, "GetSharpness", "CurrentSharpness", instanceID);
}

int CRenderingControl::getVerticalKeystone (QString const & rendererUUID, unsigned instanceID)
{
  return getUInt1 (rendererUUID, "GetVerticalKeystone", "CurrentVerticalKeystone", instanceID);
}

int CRenderingControl::getVolumeDB (QString const & rendererUUID, QString const & channel, unsigned instanceID)
{
  return getVolume (rendererUUID, "GetVolumeDB", channel, instanceID);
}

QPair<int, int> CRenderingControl::getVolumeDBRange (QString const & rendererUUID, QString const & channel, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  QList<CControlPoint::TArgValue> args;
  args.reserve (2);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue ("Channel", channel);
  args << CControlPoint::TArgValue ("MinValue", QString::null);
  args << CControlPoint::TArgValue ("MaxValue", QString::null);
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, "GetVolumeDBRange", args);
  QPair<int, int> values;
  if (actionInfo.succeeded ())
  {
    values.first  = (*(args.end () - 2)).second.toInt ();
    values.second = args.last ().second.toInt ();
  }

  return values;
}

bool CRenderingControl::setBlueVideoBlackLevel (QString const & rendererUUID, unsigned value, unsigned instanceID)
{
  return setUInt1 (rendererUUID, "SetBlueVideoBlackLevel", "DesiredBlueVideoBlackLevel", value, instanceID);
}

bool CRenderingControl::setBlueVideoGain (QString const & rendererUUID, unsigned value, unsigned instanceID)
{
  return setUInt1 (rendererUUID, "SetBlueVideoGain", "DesiredBlueVideoGain", value, instanceID);
}

bool CRenderingControl::setBrightness (QString const & rendererUUID, unsigned value, unsigned instanceID)
{
  return setUInt1 (rendererUUID, "SetBrightness", "DesiredBrightness", value, instanceID);
}

bool CRenderingControl::setColorTemperature (QString const & rendererUUID, unsigned value, unsigned instanceID)
{
  return setUInt1 (rendererUUID, "SetColorTemperature", "DesiredColorTemperature", value, instanceID);
}

bool CRenderingControl::setContrast (QString const & rendererUUID, unsigned value, unsigned instanceID)
{
  return setUInt1 (rendererUUID, "SetContrast", "DesiredContrast", value, instanceID);
}

bool CRenderingControl::setGreenVideoBlackLevel (QString const & rendererUUID, unsigned value, unsigned instanceID)
{
  return setUInt1 (rendererUUID, "SetGreenVideoBlackLevel", "DesiredGreenVideoBlackLevel", value, instanceID);
}

bool CRenderingControl::setGreenVideoGain (QString const & rendererUUID, unsigned value, unsigned instanceID)
{
  return setUInt1 (rendererUUID, "SetGreenVideoGain", "DesiredGreenVideoGain", value, instanceID);
}

bool CRenderingControl::setHorizontalKeystone (QString const & rendererUUID, int value, unsigned instanceID)
{
  return setInt1 (rendererUUID, "SetHorizontalKeystone", "DesiredHorizontalKeystone", value, instanceID);
}

bool CRenderingControl::setLoudness (QString const & rendererUUID, bool value, QString const & channel, unsigned instanceID)
{
  return setBoolChannel (rendererUUID, "SetLoudness", value, "DesiredLoudness", channel, instanceID);
}

bool CRenderingControl::setRedVideoBlackLevel (QString const & rendererUUID, unsigned value, unsigned instanceID)
{
  return setUInt1 (rendererUUID, "SetRedVideoBlackLevel", "DesiredRedVideoBlackLevel", value, instanceID);
}

bool CRenderingControl::setRedVideoGain (QString const & rendererUUID, unsigned value, unsigned instanceID)
{
  return setUInt1 (rendererUUID, "SetRedVideoGain", "DesiredRedVideoGain", value, instanceID);
}

bool CRenderingControl::setSharpness (QString const & rendererUUID, unsigned value, unsigned instanceID)
{
  return setInt1 (rendererUUID, "SetSharpness", "DesiredSharpness", value, instanceID);
}

bool CRenderingControl::setVerticalKeystone (QString const & rendererUUID, int value, unsigned instanceID)
{
  return setInt1 (rendererUUID, "SetVerticalKeystone", "DesiredVerticalKeystone", value, instanceID);
}

bool CRenderingControl::setVolumeDB (QString const & rendererUUID, int value, QString const & channel, unsigned instanceID)
{
  return setVolume (rendererUUID, "SetVolumeDB", value, channel, instanceID);
}

unsigned CRenderingControl::getUInt1 (QString const & rendererUUID, QString const & action, QString const & arg, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  QList<CControlPoint::TArgValue> args;
  args.reserve (3);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue (arg, QString::null);
  unsigned    value      = 0;
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, action, args);
  if (actionInfo.succeeded ())
  {
    value = args.last ().second.toUInt ();
  }

  return value;
}

int CRenderingControl::getInt1 (QString const & rendererUUID, QString const & action, QString const & arg, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  QList<CControlPoint::TArgValue> args;
  args.reserve (3);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue (arg, QString::null);
  int         value      = 0;
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, action, args);
  if (actionInfo.succeeded ())
  {
    value = args.last ().second.toInt ();
  }

  return value;
}

bool CRenderingControl::setUInt1 (QString const & rendererUUID, QString const & action, QString const & arg, unsigned value, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  QList<CControlPoint::TArgValue> args;
  args.reserve (3);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue (arg, QString::number (value));
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, action, args);
  return actionInfo.succeeded ();
}

bool CRenderingControl::setInt1 (QString const & rendererUUID, QString const & action, QString const & arg, int value, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  QList<CControlPoint::TArgValue> args;
  args.reserve (3);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue (arg, QString::number (value));
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, action, args);
  return actionInfo.succeeded ();
}

bool CRenderingControl::setVolume (QString const & rendererUUID, QString const & action, int value, QString const & channel, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  QList<CControlPoint::TArgValue> args;
  args.reserve (3);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue ("Channel", channel);
  QString volume = QString ("%1").arg (value);
  args << CControlPoint::TArgValue ("DesiredVolume", volume);
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, action, args);
  return actionInfo.succeeded ();
}

int CRenderingControl::getVolume (QString const & rendererUUID, QString const & action, QString const & channel, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  QList<CControlPoint::TArgValue> args;
  args.reserve (3);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue ("Channel", channel);
  args << CControlPoint::TArgValue ("CurrentVolume", QString::null);
  int         volume     = 0;
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, action, args);
  if (actionInfo.succeeded ())
  {
    volume = args.last ().second.toInt ();
  }

  return volume;
}

bool CRenderingControl::setBoolChannel (QString const & rendererUUID, QString const & action, bool value, QString const & arg, QString const & channel, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  QList<CControlPoint::TArgValue> args;
  args.reserve (3);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue ("Channel", channel);
  args << CControlPoint::TArgValue (arg, value ? "1" : "0");
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, action, args);
  return actionInfo.succeeded ();
}

bool CRenderingControl::getBoolChannel (QString const & rendererUUID, QString const & action, QString const & arg, QString const & channel, unsigned instanceID)
{
  Q_ASSERT (m_cp != nullptr);
  QList<CControlPoint::TArgValue> args;
  args.reserve (3);
  args << CControlPoint::TArgValue ("InstanceID", QString::number (instanceID));
  args << CControlPoint::TArgValue ("Channel", channel);
  args << CControlPoint::TArgValue (arg, QString::null);
  bool        value      = false;
  CActionInfo actionInfo = m_cp->invokeAction (rendererUUID, action, args);
  if (actionInfo.succeeded ())
  {
    value = toBool (args.last ().second);
  }

  return value;
}
