#ifndef RENDERING_CONTROL_HPP
#define RENDERING_CONTROL_HPP

#include "control.hpp"
#include <QStringList>

START_DEFINE_UPNP_NAMESPACE

class CControlPoint;
class CActionInfo;

/*! \brief The class CRenderingControl is a wrapper that manages the urn:schemas-upnp-org:service:RenderingControl:1
 * service actions.
 */
class UPNP_API CRenderingControl : public CControl
{
public:
  /*! Default contructor. */
  CRenderingControl () {}

  /*! Contructs a CRenderingControl with a control point. */
  CRenderingControl (CControlPoint* cp)  : CControl (cp) {}

  /*! Returns list of presets.
   * Service urn:schemas-upnp-org:service:RenderingControl.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The list of presets.
   */
  QStringList getListPresets (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Selects preset.
   * \param rendererUUID: Renderer uuid.
   * \param preset: The preset to select.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return true in case of success. . Otherwise false.
   */
  bool selectPreset (QString const & rendererUUID, QString const & preset, unsigned instanceID = 0);

  /*! Gets volume.
   * \param rendererUUID: Renderer uuid.
   * \param channel: The channel. Default value "Master".
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The current volume.
   */
  int getVolume(QString const & rendererUUID, QString const & channel = "Master", unsigned instanceID = 0);

  /*! Gets mute state.
   * \param rendererUUID: Renderer uuid.
   * \param channel: The channel. Default value "Master".
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The mute state.
   */
  bool getMute (QString const & rendererUUID, QString const & channel = "Master", unsigned instanceID = 0);

  /*! Sends mute state.
   * \param rendererUUID: Renderer uuid.
   * \param value: True for mute, false for sound.
   * \param channel: The channe.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return true in case of success. . Otherwise false.
   */
  bool setMute (QString const & rendererUUID, bool value, QString const & channel = "Master", unsigned instanceID = 0);

  /*! Sends volume.
  * \param rendererUUID: Renderer uuid.
  * \param instanceID: The value returned by GetCurrentConnectionIDs.
  * \param value: The desired value volume.
  * \param channel: The channe.
  * \return true in case of success. . Otherwise false.
  * \remark Must be in [min-max] interval. See CControlPoint::stateVariableLimits.
  */
  bool setVolume (QString const & rendererUUID, unsigned value, QString const & channel = "Master", unsigned instanceID = 0);

  /*! Returns the minimum output intensity of blue.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return the minimum output intensity of blue.
   */
  unsigned getBlueVideoBlackLevel (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Returns the current setting of the blue “gain”.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The "gain".
   * \remark Not tested.
   */
  unsigned getBlueVideoGain (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Returns the current brightness.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The brightness.
   * \remark Not tested.
   */
  unsigned getBrightness (QString const & rendererUUID, unsigned instanceID = 0);

 /*! Returns the current color temperature.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return Thecolor temperature.
   * \remark Not tested.
   */
  unsigned getColorTemperature (QString const & rendererUUID, unsigned instanceID = 0);

 /*! Returns the current contrast.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The contrast.
   * \remark Not tested.
   */
  unsigned getContrast (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Returns the minimum output intensity of green.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return the minimum output intensity of green.
   */
  unsigned getGreenVideoBlackLevel (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Returns the current setting of the green “gain”.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return the "gain".
   * \remark Not tested.
   */
  unsigned getGreenVideoGain (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Returns the current horizontal keystone.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The "gain".
   * \remark Not tested.
   */
  int getHorizontalKeystone (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Returns the loudness state.
   * \param rendererUUID: Renderer uuid.
   * \param channel: The channel. Default value "Master".
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The loudness state.
   * \remark Not tested.
   */
  bool getLoudness (QString const & rendererUUID, QString const & channel = "Master", unsigned instanceID = 0);

  /*! Returns the minimum output intensity of red.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return the minimum output intensity of red.
   * \remark Not tested.
   */
  unsigned getRedVideoBlackLevel (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Returns the current setting of the red “gain”.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return the "gain".
   * \remark Not tested.
   */
  unsigned getRedVideoGain (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Returns the current sharpness.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The sharpness.
   * \remark Not tested.
   */
  unsigned getSharpness (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Returns the current vertical keystone.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The vertical keystone.
   * \remark Not tested.
   */
  int getVerticalKeystone (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Returns the current volume in 1/256 dB.
   * \param rendererUUID: Renderer uuid.
   * \param channel: The channel. Default value "Master".
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The volume in 1/256 db.
   */
  int getVolumeDB (QString const & rendererUUID, QString const & channel = "Master", unsigned instanceID = 0);

  /*! Returns the current volume in 1/256 dB range.
   * \param rendererUUID: Renderer uuid.
   * \param channel: The channel. Default value "Master".
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The minimum in first and the maximum in second.
   */
  QPair<int, int> getVolumeDBRange (QString const & rendererUUID, QString const & channel = "Master", unsigned instanceID = 0);

  /*! Sets the minimum output intensity of blue.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \param value: The intensity.
   * \return the minimum output intensity of green.
   */
  bool setBlueVideoBlackLevel (QString const & rendererUUID, unsigned value, unsigned instanceID = 0);

  /*! Set the current setting of the blue “gain”.
   * \param rendererUUID: Renderer uuid.
   * \param value: The "gain".
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return True for success.
   * \remark Not tested.
   */
  bool setBlueVideoGain (QString const & rendererUUID, unsigned value, unsigned instanceID = 0);

  /*! Sets the current brightness.
   * \param rendererUUID: Renderer uuid.
   * \param value: The brightness.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return True for success.
   * \remark Not tested.
   */
  bool setBrightness (QString const & rendererUUID, unsigned value, unsigned instanceID = 0);

  /*! Sets the current color temperature.
   * \param rendererUUID: Renderer uuid.
   * \param value: The color temperature.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return True for success.
   * \remark Not tested.
   */
  bool setColorTemperature (QString const & rendererUUID, unsigned value, unsigned instanceID = 0);

  /*! Set the current contrast”.
   * \param rendererUUID: Renderer uuid.
   * \param value: The contrast”.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return True for success.
   * \remark Not tested.
   */
  bool setContrast (QString const & rendererUUID, unsigned value, unsigned instanceID = 0);

  /*! Sets the minimum output intensity of green.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \param value: The intensity.
   * \return True for success.
   * \remark Not tested.
   */
  bool setGreenVideoBlackLevel (QString const & rendererUUID, unsigned value, unsigned instanceID = 0);

  /*! Set the current setting of the green “gain”.
   * \param rendererUUID: Renderer uuid.
   * \param value: The "gain".
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return True for success.
   * \remark Not tested.
   */
  bool setGreenVideoGain (QString const & rendererUUID, unsigned value, unsigned instanceID = 0);

  /*! Sets the current horizontal keystone.
   * \param rendererUUID: Renderer uuid.
   * \param value: The horizontal keystone.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return True for success.
   * \remark Not tested.
   */
  bool setHorizontalKeystone (QString const & rendererUUID, int value, unsigned instanceID = 0);

  /*! Activates ot inactivates the loudness.
   * \param rendererUUID: Renderer uuid.
   * \param value: True to activated, false to inativated.
   * \param channel: The channel.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return True for success.
   * \remark Not tested.
   */
  bool setLoudness (QString const & rendererUUID, bool value, QString const & channel = "Master", unsigned instanceID = 0);

  /*! Sets the minimum output intensity of red.
   * \param rendererUUID: Renderer uuid.
   * \param value: The intensity.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.*
   * \return True for success.
   * \remark Not tested.
   */
  bool setRedVideoBlackLevel (QString const & rendererUUID, unsigned value, unsigned instanceID = 0);

  /*! Set the current setting of the red “gain”.
   * \param rendererUUID: Renderer uuid.
   * \param value: The "gain".
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return True for success.
   * \remark Not tested.
   */
  bool setRedVideoGain (QString const & rendererUUID, unsigned value, unsigned instanceID = 0);

  /*! Sets the current sharpness.
   * \param rendererUUID: Renderer uuid.
   * \param value: The sharpness.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return True for success.
   * \remark Not tested.
   */
  bool setSharpness (QString const & rendererUUID, unsigned value, unsigned instanceID = 0);

  /*! Sets the current vertical keystone.
   * \param rendererUUID: Renderer uuid.
   * \param value: The vertical keystone.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return True for success.
   */
  bool setVerticalKeystone (QString const & rendererUUID, int value, unsigned instanceID = 0);

 /*! Returns the current volume in 1/256 dB.
   * \param rendererUUID: Renderer uuid.
   * \param channel: The channel. Default value "Master".
   * \param value: The volume.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return True for success.
   */
  bool setVolumeDB (QString const & rendererUUID, int value, QString const & channel = "Master", unsigned instanceID = 0);

protected :
  /*! Returns the unsigned value, returned by the action.
   * This function is called by action needs one argument and returns an unsigned.
   * \param rendererUUID: Renderer uuid.
   * \param action: The action.
   * \param arg: The argument.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return the unsigned value.
   */
  unsigned getUInt1 (QString const & rendererUUID, QString const & action, QString const & arg, unsigned instanceID = 0);

  /*! Returns the int value, returned by the action.
   * This function is called by action needs one argument and returns an int.
   * \param rendererUUID: Renderer uuid.
   * \param action: The action.
   * \param arg: The argument.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return the int value.
   */
  int getInt1 (QString const & rendererUUID, QString const & action, QString const & arg, unsigned instanceID = 0);

  /*! Sets the unsigned value, to the action.
   * This function is called by action needs one argument and an unsigned value.
   * \param rendererUUID: Renderer uuid.
   * \param action: The action.
   * \param arg: The argument.
   * \param value: The unsigned value.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return True for success.
   */
  bool setUInt1 (QString const & rendererUUID, QString const & action, QString const & arg, unsigned value, unsigned instanceID = 0);

  /*! Sets the int value, to the action.
   * This function is called by action needs one argument and an int value.
   * \param rendererUUID: Renderer uuid.
   * \param action: The action.
   * \param arg: The argument.
   * \param value: The int value.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return True for success.
   */
  bool setInt1 (QString const & rendererUUID, QString const & action, QString const & arg, int value, unsigned instanceID = 0);

  /*! Sets the volume or volumeDB.
   * \param rendererUUID: Renderer uuid.
   * \param action: The action.
   * \param channel: The channel.
   * \param value: The int value.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return True for success.
   */
  bool setVolume (QString const & rendererUUID, QString const & action, int value, QString const & channel = "Master", unsigned instanceID = 0);

  /*! Returns the volume or volumeDB.
   * \param rendererUUID: Renderer uuid.
   * \param action: The action.
   * \param channel: The channel.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The value of the  volume or volumeDB.
   */
  int getVolume (QString const & rendererUUID, QString const & action, QString const & channel = "Master", unsigned instanceID = 0);

  /*! Sets the mute or loudness state.
   * \param rendererUUID: Renderer uuid.
   * \param action: GetMute or GetLoundness.
   * \param value: The value true or false.
   * \param arg: The argument.
   * \param channel: The channel.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The value of state.
   */
  bool setBoolChannel (QString const & rendererUUID, QString const & action, bool value, QString const & arg, QString const & channel = "Master", unsigned instanceID = 0);

  /*! Returns the mute or loudness state.
   * \param rendererUUID: Renderer uuid.
   * \param action: SetMute or SetLoundness.
   * \param arg: The argument.
   * \param channel: The channel.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The value of state.
   */
  bool getBoolChannel (QString const & rendererUUID, QString const & action, QString const & arg, QString const & channel = "Master", unsigned instanceID = 0);
};

} // Namespace

#endif // RENDERING_CONTROL_HPP
