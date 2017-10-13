
#include "httpserver.hpp"
#include "waitingloop.hpp"
#include "xmlhevent.hpp"
#include <QTcpSocket>
#include <QDate>

USING_UPNP_NAMESPACE

CHTTPServer::CHTTPServer (QHostAddress const & address, quint16 port, QObject* parent) : QTcpServer (parent), m_done (false)
{
  bool success = listen (address, port);
  if (success)
  {
    m_done = true;
  }
}

CHTTPServer::~CHTTPServer ()
{
}

void CHTTPServer::incomingConnection (qintptr socketDescriptor)
{
  QTcpSocket* socket     = new QTcpSocket (this);
  m_eventMessage[socket] = QPair<QByteArray, int> (QByteArray (), 0);
  socket->setSocketDescriptor (socketDescriptor);
  connect (socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

bool CHTTPServer::connectToHost (QTcpSocket* socket, QHostAddress host, quint16 port)
{
  bool success = socket->state () == QAbstractSocket::ConnectedState;
  if (!success)
  {
    CWaitingLoop connectToHostLoop (1000, QEventLoop::ExcludeUserInputEvents);
    connect (socket, SIGNAL(connected()), &connectToHostLoop, SLOT(quit()));
    socket->connectToHost (host, port);
    connectToHostLoop.exec (QEventLoop::ExcludeUserInputEvents);
    success = socket->state () == QAbstractSocket::ConnectedState;
  }

  if (success)
  {
    socket->waitForBytesWritten (2000);
    connect (socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
  }
  else
  {
    socket->deleteLater ();
  }

  return success;
}

bool CHTTPServer::httpSimpleResponse (QTcpSocket* socket, QHostAddress addressDevice, quint16 portDevice, bool reject)
{
  char const * ok             = !reject ? "HTTP/1.1 200 OK\r\n" : "HTTP/1.1 500 Internal Server Error\r\n";
  char const * date           = "DATE: %1 %2\r\n";
  char const * host           = "HOST: %1:%2\r\n";
  char const * content_length = "content-length: 0\r\n\r\n";

  QString data = ok +
                 QString (date).arg (QDate::currentDate ().toString ("ddd, dd MMM yyyy")).arg (QTime::currentTime ().toString ("hh:mm:ss")) +
                 QString (host).arg (addressDevice.toString ()).arg (portDevice) +
                 content_length;

  bool success = false;
  if (connectToHost (socket, addressDevice, portDevice))
  {
    connect (socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    QByteArray bdata = data.toUtf8 ();
    socket->setProperty ("lengthToWrite:", bdata.length ());
    success = socket->write (bdata) != -1;
    socket->flush ();
  }

  return success;
}

bool CHTTPServer::accept (QTcpSocket* socket, QHostAddress addressDevice, quint16 portDevice)
{
  return httpSimpleResponse (socket, addressDevice, portDevice, false);
}

bool CHTTPServer::reject (QTcpSocket* socket, QHostAddress addressDevice, quint16 portDevice)
{
  return httpSimpleResponse (socket, addressDevice, portDevice, true);
}

bool CHTTPServer::httpPlaylistResponse (QTcpSocket* socket, QHostAddress address, quint16 port, QByteArray const & response)
{
  bool success = false;
  if (connectToHost (socket, address, port))
  {
    connect (socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    socket->setProperty ("lengthToWrite:", response.length ());
    success = socket->write (response) != -1;
    socket->flush ();
  }

  return success;
}

void CHTTPServer::parseMessage (CHTTPParser const & httpParser, int headerLength, int contentLength,
                                QByteArray const & eventMessage, QTcpSocket* socket)
{
  QByteArray const & verb = httpParser.verb ();
  if (verb == "NOTIFY" &&
      httpParser.value ("NT") == "upnp:event" &&
      httpParser.value ("NTS") == "upnp:propchange")
  {
    m_vars.clear ();
    if (contentLength != 0)
    {
      QByteArray data = eventMessage.mid (headerLength);
      CXmlHEvent h (m_vars);
      h.parse (data);

      QString const lastChange ("LastChange");
      if (m_vars.contains (lastChange))
      {
        data = m_vars.value (lastChange).first.toUtf8 ();
        if (!data.isEmpty ())
        {
          h.setCheckProperty (false);
          h.parse (data);
        }
      }

      accept (socket, socket->peerAddress (), socket->peerPort ());
      if (!m_vars.isEmpty ())
      {
        emit eventReady (httpParser.sid ());
      }
    }
    else
    {
      reject (socket, socket->peerAddress (), socket->peerPort ());
    }
  }
  else
  {
    bool       playlist = httpParser.isPlaylistPath ();
    QByteArray response;
    if (verb == "GET")
    {
      response = getMethodResponse (playlist);
    }
    else if (verb == "HEAD")
    {
      response = headMethodResponse (playlist);
    }

    if (!response.isEmpty ())
    {
      httpPlaylistResponse (socket, socket->peerAddress (), socket->peerPort (), response);
    }
    else
    {
      reject (socket, socket->peerAddress (), socket->peerPort ());
    }
  }

  m_eventMessage.remove (socket);
}

void CHTTPServer::readyRead ()
{
  QTcpSocket*   socket       = static_cast<QTcpSocket*>(sender ());
  TMessageData& messageData  = m_eventMessage[socket];
  QByteArray&   eventMessage = messageData.first;
  eventMessage              += socket->readAll ();

  CHTTPParser httpParser (eventMessage);
  int headerLength = httpParser.headerLength ();
  if (headerLength > 0)
  {
    if (messageData.second == 0)
    { // To parser the header once.
      httpParser.parseMessage ();
      messageData.second = httpParser.contentLength ();
    }

    int contentLength = messageData.second;
    if (contentLength >= 0)
    { // CONTENT-LENGTH header is defined.
      int messageLength = eventMessage.size ();
      if (messageLength >= headerLength + contentLength)
      {
        parseMessage (httpParser, headerLength, contentLength, eventMessage, socket);
      }
    }
    else
    { // TRANSFER-ENCODING: chunked is defined; In this case messageData.second < 0 and represent
      // the current decoded length.
      messageData.second = messageData.second == -1 ? headerLength : -messageData.second;
      QByteArray hexaChunkLength;
      hexaChunkLength.reserve (16);
      bool ok            = true;
      int index          = eventMessage.indexOf ("\r\n", messageData.second), chunkLength;
      while (index != -1 && ok)
      {
        int hexaChunkLengthSize = index - messageData.second + 2;
        hexaChunkLength         = eventMessage.mid (messageData.second, hexaChunkLengthSize).trimmed ();
        chunkLength             = hexaChunkLength.toUInt (&ok, 16);
        if (ok)
        {
          eventMessage.remove (messageData.second, hexaChunkLengthSize);
          messageData.second += chunkLength - 1;
          if (chunkLength == 0)
          {
            parseMessage (httpParser, headerLength, messageData.second, eventMessage, socket);
            break;
          }

          index = eventMessage.indexOf ("\r\n", messageData.second);
        }
      }

      messageData.second = -messageData.second;
    }
  }
}

void CHTTPServer::bytesWritten (qint64 bytes)
{
  QTcpSocket* socket = static_cast<QTcpSocket*>(sender ());
  qint64      length = socket->property ("lengthToWrite:").toLongLong ();
  if (length == bytes)
  {
    socket->disconnectFromHost ();
  }
}

QString CHTTPServer::playlistBaseName (QString const & name)
{
  QString temp = name;
  return temp.replace (':', '-'); // Replace uuid: by uuid-
}

QString CHTTPServer::playlistURI (QString const & name) const
{
  QString      temp = playlistBaseName (name);
  QHostAddress host = serverAddress ();
  quint16      port = serverPort ();
  return QString ("http://%1:%2/%3-%4.m3u").arg (host.toString ()).arg (port)
                                           .arg (temp).arg (m_playlistIndex++);
}

QString CHTTPServer::audioFileURI (QString const & name) const
{
  QHostAddress host = serverAddress ();
  quint16      port = serverPort ();
  return QString ("http://%1:%2/%3").arg (host.toString ()).arg (port).arg (name);
}

void CHTTPServer::setPlaylistContent (QByteArray const & content)
{
  m_playlistContent = content;
}

QByteArray CHTTPServer::time (QDateTime const & dt)
{
  return QLocale (QLocale::English, QLocale::UnitedStates).toString (dt, "ddd, dd MMMM yyyy HH:mm:ss ").toUtf8 () + "GMT";
}

QByteArray CHTTPServer::currentTime ()
{
  return time (QDateTime::currentDateTime ());
}

QByteArray CHTTPServer::headerResponse (int contentLength, char const * contentType) const
{
  if (contentType == nullptr)
  {
    contentType = "audio/x-mpegurl";
  }

  QByteArray currentTime = this->currentTime ();
  QByteArray header      = "HTTP/1.1 200 OK\r\nServer: UPnP/1.0 dmupnp/1.0.0\r\n\
Date: " + currentTime + "\r\ntransferMode.dlna.org: Streaming\r\n\
contentFeatures.dlna.org: DLNA.ORG_OP=01;DLNA.ORG_FLAGS=01700000000000000000000000000000\r\n\
CACHE-CONTROL: no-cache\r\n\
PRAGMA: no-cache\r\n\
Content-Type: ";
  header += contentType;
  header += "\r\n\"\
Accept-Ranges: bytes\r\n\
Content-Length: " + QByteArray::number (contentLength) + "\r\n\r\n";
  return header;
}

QByteArray CHTTPServer::headMethodResponse (bool internalPlaylist) const
{
  QByteArray response;
  if (internalPlaylist)
  {
    response = headerResponse (m_playlistContent.size ());
  }

  return response;
}

QByteArray CHTTPServer::getMethodResponse (bool internalPlaylist) const
{
  QByteArray response;
  if (internalPlaylist)
  {
    response = headerResponse (m_playlistContent.size ()) + m_playlistContent;
  }

  return response;
}
