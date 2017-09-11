
#include "multicastsocket.hpp"
#include "helper.hpp"
#include <QNetworkProxy>
#include <QNetworkInterface>
#ifdef Q_OS_WIN
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif
#include <cstring>

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

// This code is extracted from Herqq UPnP library
bool CMulticastSocket::joinMulticastAddress (QHostAddress const & multicastAddress)
{
  QByteArray   blha = multicastAddress.toString ().toLatin1 ();
  char const * clha = blha.constData ();

  QByteArray   biface = localHostAddress ().toString ().toLatin1 ();
  char const * ciface = biface.constData ();

  struct ip_mreq mreq;
  std::memset (&mreq, 0, sizeof (ip_mreq));
  mreq.imr_multiaddr.s_addr = ::inet_addr (clha);
  mreq.imr_interface.s_addr = ::inet_addr (ciface);
  ::inet_pton (AF_INET, clha,   &mreq.imr_multiaddr.s_addr);
  ::inet_pton (AF_INET, ciface, &mreq.imr_interface.s_addr);
  qintptr   sd       = socketDescriptor ();
  int const mreqSize = sizeof (mreq);
  return ::setsockopt (sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<char*>(&mreq), mreqSize) >= 0;
}

bool CMulticastSocket::initialize (QHostAddress multicastAddress, quint16 multicastPort)
{
  bool success = false;

  // Bind multicast socket
  if (bind (QHostAddress::AnyIPv4, multicastPort, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress))
  {
#define _JOINMULTICAST  3
    // Add member to multicast group
    // Use local functions because joinMulticastGroup has a problem on qt 4.6->5.6.
    // joinMulticastGroup (hostAddress, ifaces[iface]) use the first address of the interface.Probably the second work.
#if _JOINMULTICAST == 1
    if (joinMulticastAddress (multicastAddress))
#elif _JOINMULTICAST == 2
    QHostAddress             loalIp = localHostAddress ();
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces ();
    int                      iface  = -1;
    for (int iIFace = 0; iIFace < ifaces.size () && iface == -1; ++iIFace)
    {
      QList<QNetworkAddressEntry> addrEntries = ifaces[iIFace].addressEntries ();
      for (int iAddrEntry = 0; iAddrEntry < addrEntries.size (); ++iAddrEntry)
      {
        QHostAddress ip = addrEntries[iAddrEntry].ip ();
        if (ip == loalIp)
        {
          iface = iIFace;
          break;
        }
      }
    }

    if (joinMulticastGroup (hostAddress, ifaces[iface]))
#else
    if (joinMulticastGroup (multicastAddress))
#endif
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

