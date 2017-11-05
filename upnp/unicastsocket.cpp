
#include "unicastsocket.hpp"
#include "helper.hpp"

USING_UPNP_NAMESPACE

CUnicastSocket::CUnicastSocket (QObject* parent) : CUpnpSocket (parent)
{
}

CUnicastSocket::~CUnicastSocket ()
{
}

quint16 CUnicastSocket::bind (QHostAddress addr)
{
  quint16 port;
  // The range is specified by the UDA 1.1 standard
  for (port = 49152; port < 65535; ++port)
  {
    if (QUdpSocket::bind (addr, port, QAbstractSocket::DontShareAddress))
    {
      break;
    }
  }

  if (port == 65535 || state () != QUdpSocket::BoundState)
  {
    qDebug () << "CUnicastSocket::bind (no ports free):" << addr.toString ().toLatin1 () + ':'+ QByteArray::number (port);
    port = 0;
  }

  return port;
}
