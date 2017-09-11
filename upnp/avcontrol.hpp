#ifndef AVCONTROL_HPP
#define AVCONTROL_HPP

#include "renderingcontrol.hpp"
#include "connectionmanager.hpp"
#include "avtransport.hpp"
#include "contentdirectory.hpp"

START_DEFINE_UPNP_NAMESPACE

/*! \brief The class CAVControl encapsulates the action managers requisite to build an
 * UPnP/AV control point.
 *
 * You can use UPnP/AV action managers individually.
 */
class CAVControl
{
public:
  /*! Default constructor. */
  CAVControl ();

  /*! Constructor. */
  CAVControl (CControlPoint* cp);

  /*! Sets the application control point. */
  void setControlPoint (CControlPoint* cp);

  /*! Returns the control point. */
  CControlPoint* controlPoint ();

  /*! Returns the connection manager. */
  CConnectionManager& connectionManager () { return m_cm; }

  /*! Returns the content directory manager. */
  CContentDirectory& contentDirectory () { return m_cd; }

  /*! Returns the AVTransport manager. */
  CAVTransport& avTransport () { return m_tr; }

  /*! Returns the rendering control manager. */
  CRenderingControl& renderingControl () { return m_rc; }

protected :
  CConnectionManager m_cm; //!< The connection manager.
  CContentDirectory m_cd; //!< The content directory manager.
  CAVTransport m_tr; //!< The content AVtransport manager.
  CRenderingControl m_rc; //!< The content rendering control manager.
};

} // Namespace

#endif // AVCONTROL_HPP
