#ifndef AVTRANSPORT_HPP
#define AVTRANSPORT_HPP

#include "control.hpp"
#include "positioninfo.hpp"
#include "transportsettings.hpp"
#include "transportinfo.hpp"
#include "mediainfo.hpp"
#include "devicecaps.hpp"

START_DEFINE_UPNP_NAMESPACE

class CControlPoint;
class CDidlItem;
class CActionInfo;

/*! \brief A wrapper that manages the urn:schemas-upnp-org:service:AVTransport:1
 * service actions.
 */
class UPNP_API CAVTransport : public CControl
{
public:
  /*! Default constructor. */
  CAVTransport () {}

  /*! Constructor.
   * \param cp: The control point of the application.
   */
  CAVTransport (CControlPoint* cp) : CControl (cp) {}

  /*! Invokes GetCurrenTransportActions.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return Current possible actions.
   */
  QStringList getCurrentTransportActions (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Invokes GetPositionInfo .
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The current position info.
   */
  CPositionInfo getPositionInfo (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Invokes GetTransportSettings (NORMAL, REPEAT_ALL...)
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The current transport settings.
   */
  CTransportSettings getTransportSettings (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Invokes GetDevicesCapabilities.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The device capabilities.
   */
  CDeviceCaps getDeviceCaps (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Invokes GetTransportInfo.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The transport info.
   */
  CTransportInfo getTransportInfo (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Invokes GetMediaInfo.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return The media info.
   */
  CMediaInfo getMediaInfo (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Invokes SetAVTransportURI.
   * \param rendererUUID: Renderer uuid.
   * \param item: The didl of the item to play (music track, movie...)
   * \param index: The index of the xml res tag.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return true in case of success. Otherwise false.
   */
  bool setAVTransportURI (QString const & rendererUUID, CDidlItem const & item, int index = 0, unsigned instanceID = 0);

  /*! Invokes SetAVTransportURI.
   * This function calls the "SetAVTransportURI" action with an address that defines a playlist.
   * The playlist is created from items. You can verify if the renderer accepts playlists by calling the function
   * CDevice::canManagePlaylists.
   * For this function, the server is the HTTPSever of this library. It send the formatted playlist
   * to the renderer when resquested.
   * \param rendererUUID: Renderer uuid.
   * \param playlistName: The name of the playlist which will be displayed by the renderer.
   * \param items: The items to creates the playlist.
   * \param format: The playlist format. Actually only m3u and m3u8 are supported.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return true in case of success. Otherwise false.
   * \remark No playlist format verification is performed.
   */
  bool setAVTransportURI (QString const & rendererUUID, QString const & playlistName,
                          QList<CDidlItem::TPlaylistElem> const & items,
                          CDidlItem::EPlaylistFormat format = CDidlItem::M3u,
                          unsigned instanceID = 0);

  /*! Invokes SetAVTransportURI.
   * \param rendererUUID: Renderer uuid.
   * \param uri: The uri to play.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return true in case of success. Otherwise false.
   * \remark No file format verification is performed.
   */
  bool setAVTransportURI (QString const & rendererUUID, QString const & uri, unsigned instanceID = 0);

  /*! Invokes SetNextAVTransportURI.
   * \param rendererUUID: Renderer uuid.
   * \param item: The CDidlItem.
   * \param index: The res element index.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return true in case of success. Otherwise false.
   * \remark No file format verification is performed.
   */
  bool setNextAVTransportURI (QString const & rendererUUID, CDidlItem const & item, int index = 0, unsigned instanceID = 0);

  /*! Invokes SetNextAVTransportURI.
   * \param rendererUUID: Renderer uuid.
   * \param uri: The uri to play.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return true in case of success. Otherwise false.
   * \remark No file format verification is performed.
   */
  bool setNextAVTransportURI (QString const & rendererUUID, QString const & uri, unsigned instanceID = 0);

  /*! Invokes SetPlayMode.
   * \param rendererUUID: Renderer uuid.
   * \param mode: The play mode (NORMAL, REPEAT_ALL, RANDOM, ...)
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return true in case of success. Otherwise false.
   * \remark No file format verification is performed.
   */
  bool setPlayMode (QString const & rendererUUID, QString const & mode, unsigned instanceID = 0);

  /*! Invokes Play.
  * \param rendererUUID: Renderer uuid.
  * \param instanceID: The value returned by GetCurrentConnectionIDs.
  * \return true in case of success. Otherwise false.
  */
  bool play (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Invokes Pause.
  * \param rendererUUID: Renderer uuid.
  * \param instanceID: The value returned by GetCurrentConnectionIDs.
  * \return true in case of success. Otherwise false.
  */
  bool pause (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Invokes Stop.
  * \param rendererUUID: Renderer uuid.
  * \param instanceID: The value returned by GetCurrentConnectionIDs.
  * \param instanceID: The value returned by GetCurrentConnectionIDs.
  * \return true in case of success. Otherwise false.
  */
  bool stop (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Invokes SetPPS.
  * \param rendererUUID: Renderer uuid.
  * \param actionName: Action name (Play, Pause, Stop).
  * \param instanceID: The value returned by GetCurrentConnectionIDs.
  * \return true in case of success. Otherwise false.
  */
  bool setPPS (QString const & rendererUUID, QString const & actionName, unsigned instanceID = 0);

  /*! Sends next.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return true in case of success. Otherwise false.
   */
  bool next (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Sends previous.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \param rendererUUID: Renderer uuid.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return true in case of success. Otherwise false.
   */
  bool previous (QString const & rendererUUID, unsigned instanceID = 0);

  /*! Sends seek action to time position.
   * \param rendererUUID: Renderer uuid.
   * \param timePosition: The position to go. If timePosition="Beginning", the seeking position is 0:00:00.000.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return true in case of success. Otherwise false.
   */
  bool seek (QString const & rendererUUID, QString const & timePosition, unsigned instanceID = 0);

  /*! Sends seek action to track number.
   * \param rendererUUID: Renderer uuid.
   * \param iTrack: Track number start at 1.
   * \param instanceID: The value returned by GetCurrentConnectionIDs.
   * \return true in case of success. Otherwise false.
   */
  bool seek (QString const & rendererUUID, int iTrack, unsigned instanceID = 0);
};


} // Namespace

#endif // AVTRANSPORT_HPP
