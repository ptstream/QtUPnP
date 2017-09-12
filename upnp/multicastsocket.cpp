
#include "multicastsocket.hpp"

USING_UPNP_NAMESPACE

CMulticastSocket::CMulticastSocket (QObject* parent) : CUpnpSocket (parent)
{
}

CMulticastSocket::~CMulticastSocket ()
{
  if (state () == QUdpSocket::BoundState)
  {
    bool ok = leaveMulticastGroup (m_joinAddress);
    if (!ok)
    {
      qDebug () << "CMulticastSocket::~CMulticastSocket (leaveMulticastGroup):" << m_joinAddress.toString ().toLatin1 ();
    }
  }
}

bool CMulticastSocket::initialize (QHostAddress multicastAddress, quint16 multicastPort)
{
  bool success = false;

  // Bind multicast socket
  if (bind (QHostAddress::AnyIPv4, multicastPort, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress))
  {
    if (joinMulticastGroup (multicastAddress))
    {
      m_joinAddress = multicastAddress;
      success       = true;
    }
    else
    {
      qDebug () << "CMulticastSocket::initialize (joinMulticast):" << multicastAddress.toString ().toLatin1 ();
    }
  }
  else
  {
    qDebug () << "CMulticastSocket::initialize (bind port):" << QByteArray::number (multicastPort);
  }

  return success;
}

bool CMulticastSocket::initialize6 (QHostAddress multicastAddress, quint16 multicastPort)
{
  bool success = false;
  if (bind (QHostAddress::AnyIPv6, multicastPort, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress))
  {
    if (joinMulticastGroup (multicastAddress))
    {
      m_joinAddress = multicastAddress;
      success       = true;
    }
    else
    {
      qDebug () << "CMulticastSocket::initialize6 (joinMulticast):" << multicastAddress.toString ().toLatin1 ();
    }
  }
  else
  {
    qDebug () << "CMulticastSocket::initialize6 (bind port):" << QByteArray::number (multicastPort);
  }

  return success;
}

