#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include "control.hpp"
#include "connectioninfo.hpp"
#include <QVector>

START_DEFINE_UPNP_NAMESPACE

class CControlPoint;
class CActionInfo;

/*! \brief A wrapper that manages the urn:schemas-upnp-org:service:ConnectionManager:1
 *  service actions.
 */
class CConnectionManager : public CControl
{
public:
  /*! Defines the 2 directions for the protocols. */
  enum EProtocolDir { Source, Sink };

  /*! Default constructor. */
  CConnectionManager () {}

  /*! Constructor
   * \param cp: The control point of the application.
   */
  CConnectionManager (CControlPoint* cp) : CControl (cp) {}

  /*! Invokes the GetCurrentConnectionInfo action.
   * \param deviceUUID: Device uuid.
   * \return The connection info.
   */
  CConnectionInfo getCurrentConnectionInfo (QString const & deviceUUID);

  /*! Invokes the >GetProtocolInfos action.
   * \param deviceUUID: Renderer uuid.
   * \return protocol info.
   */
  QVector<QStringList> getProtocolInfos (QString const & deviceUUID);

  /*! Invokes the GetCurrentConnectionIDs action.
   * \param deviceUUID: Renderer uuid.
   * \return Current connection IDs.
   */
  QStringList getCurrentConnectionIDs (QString const & deviceUUID);
};

} // Namespace

#endif // CONNECTION_MANAGER_HPP
