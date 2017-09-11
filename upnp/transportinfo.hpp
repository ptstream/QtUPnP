#ifndef TRANSPORT_INFO_HPP
#define TRANSPORT_INFO_HPP

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"
#include <QSharedDataPointer>

START_DEFINE_UPNP_NAMESPACE

struct STransportInfoData;

/*! \brief The CTransportInfo class holds information about GetTransportInfo action.
 *
 * See http://upnp.org/specs/av/UPnP-av-AVTransport-Service.pdf.
 *
 * \remark Use implicit Sharing QT technology.
 */
class UPNP_API CTransportInfo
{
public :
  /*! Default constructor. */
  CTransportInfo ();

  /*! Copy constructor. */
  CTransportInfo (const CTransportInfo &);

  /*! Equal operator. */
  CTransportInfo& operator = (const CTransportInfo &);

  /*! Destructor. */
  ~CTransportInfo ();

  /*! Sets the current transport state. */
  void setCurrentTransportState (QString const & state);

  /*! Sets the current transport status. */
  void setCurrentTransportStatus (QString const & status);

  /*! Sets the current speed. */
  void setCurrentSpeed (QString const & speed);

  /*! Returns the current transport state. */
  QString const & currentTransportState () const;

  /*! Returns the current transport status. */
  QString const & currentTransportStatus () const;

  /*! Returns the current speed. */
  QString const & currentSpeed () const;

private:
  QSharedDataPointer<STransportInfoData> m_d; //!< Shared data pointer.
};

} //Namespace

#endif // TRANSPORT_INFO_HPP
