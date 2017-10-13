
#include "initialdiscovery.hpp"
#include "upnpsocket.hpp"
#include "waitingloop.hpp"
#include <QTimerEvent>

USING_UPNP_NAMESPACE

CInitialDiscovery::CInitialDiscovery (CUpnpSocket* socket, QHostAddress const & hostAddress, quint16 port) :
                                      m_socket (socket), m_hostAddress (hostAddress), m_port (port), m_mx (2),
                                      m_idTimer (0)
{
}

bool CInitialDiscovery::discover (bool changeMX, char const * uuid)
{
  bool success = m_socket->discover (m_hostAddress, m_port, m_mx, uuid);
  if (success)
  {
    CWaitingLoop::wait (m_discoveryPause, QEventLoop::AllEvents);
    if (changeMX)
    {
      m_mx = std::rand () % 5 + 1; // Delay [1 5] See UPnP documentation.
    }
  }

  return success;
}

void CInitialDiscovery::timerEvent (QTimerEvent* event)
{
  if (event->timerId () == m_idTimer)
  {
    quit ();
  }
}
