#ifndef INITIAL_DISCOVERY_HPP
#define INITIAL_DISCOVERY_HPP 1

#include "using_upnp_namespace.hpp"
#include <QEventLoop>
#include <QHostAddress>

START_DEFINE_UPNP_NAMESPACE

class CUpnpSocket;

/*! \brief Provides methods to discover the UPnP devices. */
class CInitialDiscovery : public QEventLoop
{
  Q_OBJECT

public :
  /*! Constructor.
   * \param socket: The socket to send discovery messages.
   * \param hostAddress: The ip address.
   * \param port: The port generally 1900.
   */
  CInitialDiscovery (CUpnpSocket* socket, QHostAddress const & hostAddress, quint16 port);

  /*! Launchs discovery.
   * \param changeMX: True means the MX of the discovery message is changed at each sended message.
   * \param uuid: The destination uuid.
   */
  bool discover (bool changeMX = false, char const * uuid = nullptr);

  /*! Sets the discovery pause in milliseconds. */
  void setDiscoveryPause (int pause) { m_discoveryPause = pause; }

  /*! Restore the MX value at the default value (3s). */
  void restoreDefaultMX () { m_mx = 3; }

  /*! Sets the socket. */
  inline void setSocket (CUpnpSocket* socket);

  /*! Sets the ip address. */
  inline void setHostAddress (QHostAddress const & hostAddress);

  /*! Sets the port. */
  inline void setPort (quint16 port);

  /*! Returns the port. */
  inline quint16 port () const;

  /*! Returns the current MX value. */
  inline quint16 mx () const;

protected:
  /*! Wrapper for timer event. */
  virtual void timerEvent (QTimerEvent* event);

protected :
  CUpnpSocket* m_socket; //!< The socket.
  QHostAddress m_hostAddress; //!< ip address.
  quint16      m_port = 0; //!< The port.
  qint16       m_mx = 5; //!< The current MX value.
  int          m_discoveryPause = 0; //!< Defualt discovery pause between each sent message (500ms).
  int          m_idTimer; //<! The id timer for timeout.
};

void CInitialDiscovery::setSocket (CUpnpSocket* socket)
{
  m_socket = socket;
}

void CInitialDiscovery::setHostAddress (QHostAddress const & hostAddress)
{
  m_hostAddress = hostAddress;
}

void CInitialDiscovery::setPort (quint16 port)
{
  m_port = port;
}

quint16 CInitialDiscovery::port () const
{
  return m_port;
}

quint16 CInitialDiscovery::mx () const
{
  return m_mx;
}

} // Namespace

#endif //_INITIAL_DISCOVERY_HPP
