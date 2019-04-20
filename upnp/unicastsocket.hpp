#ifndef UNICAST_SOCKET_HPP
#define UNICAST_SOCKET_HPP 1

#include "upnpsocket.hpp"

START_DEFINE_UPNP_NAMESPACE

/*! \brief Provides the mechanism to manage unicast sockets. */
class CUnicastSocket : public CUpnpSocket
{
public :
  /*! Default constructor. */
  CUnicastSocket (QObject* parent = nullptr);

  /*! Destructor. */
  virtual ~CUnicastSocket ();

  /*! Binds to IPV4 address on addr.
   * \param addr: The host address.
   * \return The port or 0 in case of failure.
  */
  quint16 bind (QHostAddress const & addr);
};

} // End namespace

#endif
