#ifndef MULTICAST_SOCKET_HPP
#define MULTICAST_SOCKET_HPP 1

#include "upnpsocket.hpp"

START_DEFINE_UPNP_NAMESPACE

/*! \brief Provides the mechanism to manage multicast sockets. */
class CMulticastSocket : public CUpnpSocket
{
public :
  /*! Default constructor. */
  CMulticastSocket (QObject* parent = nullptr);

  /*! Destructor. */
  virtual ~CMulticastSocket ();

  /*! Binds to IPV4 address on port multicastPort and join the multicast group.
   * \param bindAddr: Generally QHostAddress::AnyIPv4 ou QHostAddress::AnyIPv6.
   * \param group: Generally 239.255.255.250 or FF02::C.
   * \return True in case of success.
   */
  bool initialize (QHostAddress const & bindAddr,  QHostAddress const & group);

  static QHostAddress const upnpMulticastAddr; //!< Standard multicast IPV4 address.
  static QHostAddress const upnpMulticastAddr6; //!< Standard multicast IPV4 address.
  static quint16 const upnpMulticastPort; //!< Standard multicats  port.

private :
  QHostAddress m_group; //!< Save the join group.
};

} // End namespace

#endif
