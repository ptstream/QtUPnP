#ifndef CONNECTION_INFO_HPP
#define CONNECTION_INFO_HPP

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"
#include <QSharedDataPointer>

START_DEFINE_UPNP_NAMESPACE

struct SConnectionInfoData;

/*! \brief The CConnectionInfo class holds information about GetCurrentConnectionInfo action.
 *
 * See http://upnp.org/specs/av/UPnP-av-ConnectionManager-Service.pdf.
 *
 * \remark Use implicit Sharing QT technology.
 */
class UPNP_API CConnectionInfo
{
public:
  /*! Default constructor. */
  CConnectionInfo ();

  /*! Copy constructor. */
  CConnectionInfo (const CConnectionInfo &);

  /*! Equal operator. */
  CConnectionInfo& operator = (const CConnectionInfo &);

  /*! Destructor. */
  ~CConnectionInfo ();

  /*! Sets the Rcs identifier. */
  void setRcsID (int id);

  /*! Sets the AVTransport identifier. */
  void setAVTransportID (int id);

  /*! Sets the protocol information. */
  void setProtocolInfo (QString const & protocol);

  /*! Sets the peer connection manager. */
  void setPeerConnectionManager (QString const & pcm);

  /*! Sets the peer connection identifier. */
  void setPeerConnectionID (int id);

  /*! Sets device the direction. */
  void setDirection (QString const & dir);

  /*! Sets device status. */
  void setStatus (QString const & st);

  /*! Returns the Rcs identifier. */
  int rcsID () const;

  /*! Returns the AVTransport identifier. */
  int avTransportID () const;

  /*! Returns the protocol information. */
  QString const & protocolInfo () const;

  /*! Returns the peer connection manager. */
  QString const & peerConnectionManager () const;

  /*! Returns the peer connection identifier. */
  int peerConnectionID () const;

  /*! Returns device the direction. */
  QString const & direction () const;

  /*! Returns device status. */
  QString const & status () const;

private:
  QSharedDataPointer<SConnectionInfoData> m_d; //!< Shared data pointer.
};

} //Namespace

#endif // CONNECTION_INFO_HPP
