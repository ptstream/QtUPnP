
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

bool CMulticastSocket::initialize (QHostAddress multicastAddress)
{
  bool success = false;
  if (bind (QHostAddress::AnyIPv4, 0, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress))
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
    qDebug () << "CMulticastSocket::initialize (bind)";
  }

  return success;
}

bool CMulticastSocket::initialize6 (QHostAddress multicastAddress)
{
  bool success = false;
  if (bind (QHostAddress::AnyIPv6, 0, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress))
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
    qDebug () << "CMulticastSocket::initialize6 (bind)";
  }

  return success;
}

