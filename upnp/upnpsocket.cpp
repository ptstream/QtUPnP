
#include "upnpsocket.hpp"
#include "httpparser.hpp"
#include "waitingloop.hpp"
#include "helper.hpp"
#include <QNetworkInterface>
#include <QNetworkProxy>
#include <QTcpSocket>

USING_UPNP_NAMESPACE

QHostAddress CUpnpSocket::m_localHostAddress;
QList<QByteArray> CUpnpSocket::m_skippedAddresses;
QList<QByteArray> CUpnpSocket::m_skippedUUIDs;

CUpnpSocket::SNDevice::SNDevice (EType type) : m_type (type)
{
}

CUpnpSocket::SNDevice::SNDevice (SNDevice const & other)
{
  *this = other;
}

CUpnpSocket::SNDevice::SNDevice (EType type, QUrl const & url, QString const & uuid) : m_type (type), m_url (url), m_uuid (uuid)
{
}

bool CUpnpSocket::SNDevice::operator == (SNDevice const & other) const
{
  return m_url == other.m_url && m_uuid == other.m_uuid;
}

CUpnpSocket::SNDevice& CUpnpSocket::SNDevice::operator = (SNDevice const & other)
{
  m_type = other.m_type;
  m_url  = other.m_url;
  m_uuid = other.m_uuid;
  return *this;
}

CUpnpSocket::CUpnpSocket (QObject* parent) : QUdpSocket (parent)
{
}

CUpnpSocket::~CUpnpSocket ()
{
}

QHostAddress CUpnpSocket::localHostAddressFromRouter ()
{
  QHostAddress hostAddr;
  QTcpSocket   tcpSocket;
  tcpSocket.connectToHost ("8.8.8.8", 53); // google DNS
  int  iter = 0;
  bool connected;
  while (iter < 4 && (connected = tcpSocket.waitForConnected (2500)))
  {
    ++iter;
  }

  if (connected)
  {
    hostAddr = tcpSocket.localAddress ();
    tcpSocket.disconnectFromHost ();
  }

  return hostAddr;
}

QHostAddress CUpnpSocket::localHostAddress (bool ipv6)
{
  if (m_localHostAddress.isNull ())
  {
    m_localHostAddress = localHostAddressFromRouter ();
    if (m_localHostAddress.isNull ())
    {
      int const fOK = QNetworkInterface::CanMulticast | QNetworkInterface::IsUp | QNetworkInterface::IsRunning;
      int const fKO = QNetworkInterface::IsLoopBack;

      QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces ();
      for (QList<QNetworkInterface>::const_iterator itFace = interfaces.begin (); itFace != interfaces.end () && m_localHostAddress.isNull (); ++itFace)
      {
        QNetworkInterface                 iFace = *itFace;
        QNetworkInterface::InterfaceFlags flags = iFace.flags ();
        if ((flags & fOK) == fOK && (flags & fKO) == 0)
        {
          QList<QNetworkAddressEntry> entries = iFace.addressEntries ();
          for (QList<QNetworkAddressEntry>::const_iterator itEnt = entries.begin (); itEnt != entries.end () && m_localHostAddress.isNull (); ++itEnt)
          {
            QHostAddress addr = (*itEnt).ip ();
            if (!ipv6 && addr.protocol () == QAbstractSocket::IPv4Protocol)
            {

              m_localHostAddress = addr;
              break;
            }
            else if (ipv6 && addr.protocol () == QAbstractSocket::IPv6Protocol)
            {
              m_localHostAddress = addr;
              break;
            }
          }
        }
      }
    }
  }

  return m_localHostAddress;
}

QByteArray const & CUpnpSocket::readDatagrams ()
{
  QByteArray datagram;
  while (hasPendingDatagrams ())
  {
    qint64 size = pendingDatagramSize ();
    if (size != 0)
    {
      datagram.resize (size);
      readDatagram (datagram.data (), size, &m_senderAddr, &m_senderPort);
      m_datagram += datagram;
    }
  }

  return m_datagram;
}

void CUpnpSocket::decodeDatagram ()
{
  char const*  separator = "\r\n\r\n";
  int          start     = 0;
  int          pos       = m_datagram.indexOf (separator, start);
  while (pos != -1)
  {
    pos                += 4;
    QByteArray datagram = m_datagram.mid (start, pos - start);
    SNDevice   device   = createDevice (datagram);
    addDevice (device);
    start = pos;
    pos   = m_datagram.indexOf (separator, start);
  }

  m_datagram.clear ();
}

CUpnpSocket::SNDevice CUpnpSocket::createDevice (QByteArray datagram)
{
  CHTTPParser parser (datagram);
  parser.parseMessage ();

  SNDevice::EType type = SNDevice::Unknown;
  if (parser.verb ().startsWith ("NOTIFY"))
  {
    type = SNDevice::Notify;
  }
  else if (datagram.startsWith ("HTTP"))
  {
    type = SNDevice::Response;
  }
  else
  {
    if (!datagram.startsWith ("M-SEARCH "))
    {
      qDebug () << "CUpnpSocket::createDevice (Unkown datagram):\n" << datagram;
    }

    return SNDevice (type);
  }

  bool       nt = false;
  QByteArray url, uuid;
  QByteArray value = parser.value ("LOCATION");
  if (!value.isEmpty ())
  {
    url = value;
  }

  value = parser.value ("USN");
  if (!value.isEmpty ())
  {
    int index = value.indexOf ("::");
    uuid      = index != -1 ? value.left (index) : value;
  }

  value = parser.value ("NT");
  if (!value.isEmpty ())
  {
    nt = true;
  }

  value = parser.value ("NTS");
  if (!value.isEmpty ())
  {
    if (value.toLower ().indexOf ("ssdp:byebye") != -1)
    {
      type = SNDevice::Byebye;
    }
  }

  QUrl qUrl;
  if (type != SNDevice::Byebye)
  {
    qUrl.setUrl (url);
    if (!qUrl.isValid ())
    {
      type = SNDevice::Unknown;
    }
    else if (type == SNDevice::Notify && !nt)
    {
      type = SNDevice::Unknown;
    }

    if (type != SNDevice::Unknown)
    {
      QString uuidTemp = uuid.toLower ();
      QString urlTemp  = url.toLower ();
      for (QList<QByteArray>::const_iterator it = m_skippedUUIDs.cbegin (), end = m_skippedUUIDs.cend (); it != end; ++it)
      {
        if (uuidTemp.contains (*it) || urlTemp.contains (*it))
        {
          type = SNDevice::Unknown;
          break;
        }
      }
    }
  }

  if (type != SNDevice::Unknown && uuid.isEmpty ())
  {
    type = SNDevice::Unknown;
  }
  else
  {
    for (QList<QByteArray>::const_iterator it = m_skippedAddresses.cbegin (), end = m_skippedAddresses.cend (); it != end; ++it)
    {
      if (url.contains (*it))
      {
        type = SNDevice::Unknown;
        break;
      }
    }
  }

  return SNDevice (type, qUrl, uuid);
}

void CUpnpSocket::addDevice (SNDevice const & device)
{
  if (device.m_type != SNDevice::Unknown)
  {
    m_devices.push_back (device);
  }
}

bool CUpnpSocket::discover (QHostAddress hostAddress, quint16 port, quint16 mx, const char* uuid)
{
  QByteArray datagram = "M-SEARCH * HTTP/1.1\r\nHOST: %1:1900\r\nMAN: \"ssdp:discover\"\r\nMX: %2\r\nST: %3\r\nUSER-AGENT: UPnP/1.0 %4/%5\r\n\r\n";
  datagram.replace ("%1", hostAddress.toString ().toLatin1 ().constData ());

  datagram.replace ("%2", std::to_string (mx).c_str ());
  if (uuid == nullptr || *uuid == '\0')
  {
    uuid = "upnp:rootdevice";
  }

  datagram.replace ("%3", uuid);
  datagram.replace ("%4", libraryName ());
  datagram.replace ("%5", libraryVersion ());
  qint64 count = writeDatagram (datagram, hostAddress, port);
  flush ();
  waitForBytesWritten (2000);
  bool success = count == datagram.size ();
  if (!success)
  {
    QString message = hostAddress.toString () + ':' + QString::number (port);
    qDebug () << "CUpnpSocket::discover (write datagram):" << message;
  }

  return success;
}

void CUpnpSocket::setSkippedAddresses (QList<QByteArray> const & addresses)
{
  for (QByteArray const & address : addresses)
  {
    QByteArray addr = address;
    if (!addr.endsWith (':'))
    {
      addr.append (':');
    }

    m_skippedAddresses << addr;
  }
}

void CUpnpSocket::addSkippedUUID (QByteArray const & uuid)
{
  m_skippedUUIDs.append (uuid.toLower ());
}
