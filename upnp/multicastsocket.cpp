
#include "multicastsocket.hpp"
#include <QNetworkInterface>

USING_UPNP_NAMESPACE

QHostAddress const CMulticastSocket::upnpMulticastAddr = QHostAddress ("239.255.255.250"); //!< Standard multicast IPV4 address.
QHostAddress const CMulticastSocket::upnpMulticastAddr6 = QHostAddress ("FF02::C"); //!< Standard multicast IPV5 address.
quint16 const  CMulticastSocket::upnpMulticastPort = 1900; //!< Standard multicast  port.

int const fOK = QNetworkInterface::CanMulticast | QNetworkInterface::IsUp | QNetworkInterface::IsRunning;
int const fKO = QNetworkInterface::IsLoopBack;

CMulticastSocket::CMulticastSocket (QObject* parent) : CUpnpSocket (parent)
{
}

CMulticastSocket::~CMulticastSocket ()
{
  bool ok = leaveMulticastGroup (m_group);
  if (!ok)
  {
    qDebug () << "CMulticastSocket::~CMulticastSocket (leaveMulticastGroup):" << m_group.toString ().toLatin1 ();
  }
}

bool CMulticastSocket::initialize (QHostAddress const & bindAddr, QHostAddress const & group)
{
  bool success = bind (bindAddr, upnpMulticastPort, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress);
  if (success)
  {
    success = joinMulticastGroup (group);
    if (!success)
    {
      qDebug () << "CMulticastSocket::initialize (joinMulticast):" << upnpMulticastAddr.toString ().toLatin1 ();
    }
    else
    {
      m_group = group;
      setSocketOption (QAbstractSocket::MulticastTtlOption, 4);
    }
  }
  else
  {
    qDebug () << "CMulticastSocket::initialize (bind)";
  }

  return success;
}
