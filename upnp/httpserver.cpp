
#include "httpserver.hpp"
#include "waitingloop.hpp"
#include "xmlhevent.hpp"
#include <QTcpSocket>
#include <QDate>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

USING_UPNP_NAMESPACE

CHTTPServer::CHTTPServer (QHostAddress const & address, quint16 port, QObject* parent) : QTcpServer (parent), m_done (false)
{
  bool success = listen (address, port);
  if (success)
  {
    m_httpsRequest.setSslConfiguration (QSslConfiguration::defaultConfiguration ());
    m_done = true;
  }
}

CHTTPServer::~CHTTPServer ()
{
  abortStreaming ();
}

void CHTTPServer::incomingConnection (qintptr socketDescriptor)
{
  QTcpSocket* socket = new QTcpSocket (this);
  socket->setSocketDescriptor (socketDescriptor);
  m_eventMessages[socket] = CHTTPParser ();
  connect (socket, &QTcpSocket::readyRead, this, &CHTTPServer::socketReadyRead);
  connect (socket, static_cast<TFctSocketError>(&QAbstractSocket::error), this, &CHTTPServer::socketError);
  connect (socket, &QTcpSocket::disconnected, this, &CHTTPServer::socketDisconnected);
  connect (socket, &QTcpSocket::bytesWritten, this, &CHTTPServer::socketBytesWritten);
}

bool CHTTPServer::connectToHost (QTcpSocket* socket)
{
  bool success = false;
  if (socket != nullptr)
  {
    success = socket->state () == QAbstractSocket::ConnectedState;
    if (!success)
    {
      CWaitingLoop connectToHostLoop (1000, QEventLoop::ExcludeUserInputEvents);
      connect (socket, SIGNAL(connected()), &connectToHostLoop, SLOT(quit()));
      socket->connectToHost (socket->peerAddress (), socket->peerPort ());
      connectToHostLoop.exec (QEventLoop::ExcludeUserInputEvents);
      success = socket->state () == QAbstractSocket::ConnectedState;
      if (!success)
      {
        socket->deleteLater ();
      }
    }
  }

  return success;
}

bool CHTTPServer::httpSimpleResponse (QTcpSocket* socket, bool reject)
{
  char const * ok             = !reject ? "HTTP/1.1 200 OK\r\n" : "HTTP/1.1 500 Internal Server Error\r\n";
  char const * date           = "DATE: %1 %2\r\n";
  char const * host           = "HOST: %1:%2\r\n";
  char const * content_length = "content-length: 0\r\n\r\n";

  QString data = ok +
                 QString (date).arg (QDate::currentDate ().toString ("ddd, dd MMM yyyy")).arg (QTime::currentTime ().toString ("hh:mm:ss")) +
                 QString (host).arg (socket->peerAddress ().toString ()).arg (socket->peerPort ()) +
                 content_length;

  QByteArray utf8 = data.toUtf8 ();
  m_writingSocketSizes.insert (socket, utf8.size ());
  return sendHttpResponse (socket, utf8);
}

bool CHTTPServer::accept (QTcpSocket* socket)
{
  return httpSimpleResponse (socket, false);
}

bool CHTTPServer::reject (QTcpSocket* socket)
{
  return httpSimpleResponse (socket, true);
}

bool CHTTPServer::sendHttpResponse (QTcpSocket* socket, char const * bytes, int cBytes)
{
  bool success = false;
  if (socket != nullptr)
  {
    success = connectToHost (socket);
    if (success)
    {
      success = socket->write (bytes, cBytes) != -1;
    }
  }

  return success;
}

bool CHTTPServer::sendHttpResponse (QTcpSocket* socket, QByteArray const & bytes)
{
  return sendHttpResponse (socket, bytes.constData (), bytes.size ());
}

void CHTTPServer::sendHttpResponse (CHTTPParser const & httpParser, QTcpSocket* socket)
{
  QByteArray const & verb = httpParser.verb ();
  if (!verb.isEmpty ())
  {
    if (verb == "NOTIFY" &&
        httpParser.value ("NT") == "upnp:event" &&
        httpParser.value ("NTS") == "upnp:propchange")
    {
      m_vars.clear ();
      if (httpParser.contentLength () != 0)
      {
        QByteArray data = httpParser.message ().mid (httpParser.headerLength ());
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

        accept (socket);
        if (!m_vars.isEmpty ())
        {
          emit eventReady (httpParser.sid ());
        }
      }
      else
      {
        reject (socket);
      }
    }
    else
    {
      CHTTPParser::EQueryType type = httpParser.queryType ();
      QByteArray              response;
      switch (type)
      {
        case CHTTPParser::Playlist :
        {
          response = headerResponse (m_playlistContent.size (), "audio/x-mpegurl") + m_playlistContent;
          m_writingSocketSizes.insert (socket, response.size ());
          sendHttpResponse (socket, response);
          break;
        }

        case CHTTPParser::Plugin :
        {
          QString request = httpParser.value (verb);
          emit mediaRequest (request);
          if (m_httpsRequest.url ().isValid ())
          {
            startStreaming (m_httpsRequest, verb, socket);
          }
          break;
        }

        default :
          qDebug () << "Unhandled http response type";
          break;
      }
    }
  }
  else
  {
    qDebug () << "Verb is empty, very strange";
  }
}

void CHTTPServer::socketReadyRead ()
{
  QTcpSocket* socket        = static_cast<QTcpSocket*>(sender ());
  CHTTPParser& parser       = m_eventMessages[socket];
  QByteArray&  eventMessage = parser.message ();
  while (socket->bytesAvailable () != 0)
  {
    eventMessage += socket->readAll ();
    if (parser.parseMessage ())
    {
      sendHttpResponse (parser, socket);
    }
  }
}

QString CHTTPServer::formatUUID (QString const & uuid)
{
  QString formattedUUID = uuid;
  if (formattedUUID.startsWith ("uuid:"))
  {
    formattedUUID[4] = '-';
  }

  return formattedUUID;
}

QString CHTTPServer::unformatUUID (QString const & uuid)
{
  QString unformattedUUID = uuid;
  if (unformattedUUID.startsWith ("uuid-"))
  {
    unformattedUUID[4] = ':';
  }

  return unformattedUUID;
}

QString CHTTPServer::serverListenAddress () const
{
  QHostAddress host = serverAddress ();
  quint16      port = serverPort ();
  return QString ("http://%1:%2/").arg (host.toString ()).arg (port);
}

QString CHTTPServer::playlistURI (QString const & name) const
{
  return QString ("http://%1:%2/playlist/%3-%4.m3u").arg (serverAddress ().toString ())
                                                    .arg (serverPort ())
                                                    .arg (formatUUID (name))
                                                    .arg (QDateTime::currentMSecsSinceEpoch ());
}

void CHTTPServer::clearPlaylist ()
{
  m_playlistName.clear ();
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

QByteArray CHTTPServer::headerResponse (qint64 contentLength, char const * contentType) const
{
  QByteArray transfer    = contentLength >= 0 ? "Content-Length: " + QByteArray::number (contentLength) : "Transfer-Encoding: chunked";
  QByteArray currentTime = this->currentTime ();
  QByteArray header      = "\
HTTP/1.1 200 OK\r\n\
Server: UPnP/1.0 QtUPnP/1.0.0\r\n\
Date: " + currentTime + "\r\n\
Cache-control: no-cache\r\n\
Content-Type: " + contentType + "\r\n\
Accept-Ranges: bytes\r\n" + transfer + "\r\n\
TransferMode.dlna.org: Streaming\r\n\
Connection: close\r\n\r\n";
  return header;
}

void CHTTPServer::socketBytesWritten (qint64 bytes)
{
  QTcpSocket* socket = static_cast<QTcpSocket*>(sender ());
  if (m_writingSocketSizes.contains (socket))
  {
    int   sizeToWrite = m_writingSocketSizes.value (socket);
    sizeToWrite      -= bytes;
    if (sizeToWrite == 0)
    {
      socket->disconnectFromHost ();
    }
    else
    {
      m_writingSocketSizes[socket] = sizeToWrite;
    }
  }
}

void CHTTPServer::socketDisconnected ()
{
  QTcpSocket* socket = static_cast<QTcpSocket*>(sender ());
  m_eventMessages.remove (socket);
  m_writingSocketSizes.remove (socket);
  socket->deleteLater ();
  if (socket == m_streamingSocket)
  {
    m_streamingSocket = nullptr;
    abortStreaming ();
  }
}

void CHTTPServer::socketError (QAbstractSocket::SocketError err)
{
  QTcpSocket* socket = static_cast<QTcpSocket*>(sender ());
  qDebug () << "stocketError:" << err << ' ' << socket->errorString ();
}

bool CHTTPServer::startStreaming (QNetworkRequest const & request, QString const & method, QTcpSocket* socket)
{
  m_startStreaming  = true;
  bool success      = false;
  if (method == "GET" || method == "HEAD")
  {
    if (m_naMgr == nullptr)
    {
      m_naMgr = new QNetworkAccessManager (this);
    }

    abortStreaming ();
    m_streamingSocket = socket;
    m_httpsReply      = method == "HEAD" ? m_naMgr->head (request) : m_naMgr->get (request);
    m_httpsReply->setReadBufferSize (m_httpsBufferSize);
    connect (m_httpsReply, static_cast<TFctNetworkReplyError>(&QNetworkReply::error), this, &CHTTPServer::httpsError);
    connect (m_httpsReply, &QNetworkReply::finished, this, &CHTTPServer::httpsFinished);
    connect (m_httpsReply, &QNetworkReply::readyRead, this, &CHTTPServer::httpsReadyRead);
    success = true;
  }

  return success;
}

void CHTTPServer::httpsError (QNetworkReply::NetworkError err)
{
  QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender ());
  QString        error = QString ("Network reply error:%1->%2->%3")
                         .arg (err).arg (reply->url ().toString ()).arg (reply->errorString ());
  qDebug () << "CHTTPServer::httpsError: " << err << " (" << error << ")";
}

void CHTTPServer::httpsFinished ()
{
  httpsReadyRead (); // Pending data if exists.
  m_httpsReply->deleteLater ();
  m_httpsReply = nullptr;
}

void CHTTPServer::httpsReadyRead ()
{
  QNetworkReply* reply = static_cast<QNetworkReply*>(sender ());
  QByteArray     data;
  if (m_startStreaming)
  {
    quint64                          contentLength        = reply->rawHeader ("content-length").toLongLong ();
    QByteArray                       contentType          = reply->rawHeader ("content-type");
    qint64                           streamingTotalLength = -1;
    QNetworkAccessManager::Operation op                   = m_httpsReply->operation ();
    if (op == QNetworkAccessManager::HeadOperation)
    {
      streamingTotalLength = contentLength;
    }

    data = headerResponse (streamingTotalLength, contentType);
    if (streamingTotalLength >= 0)
    {
      m_writingSocketSizes.insert (m_streamingSocket, data.size ());
      sendHttpResponse (m_streamingSocket, data);
      return;
    }

    m_writingSocketSizes.insert (m_streamingSocket, data.size () + contentLength);
    m_startStreaming = false;
  }

  QByteArray httpsData = reply->readAll ();
  QByteArray chunkSize = QByteArray::number (httpsData.size (), 16);
  data                += chunkSize + "\r\n" + httpsData + "\r\n";
  m_writingSocketSizes[m_streamingSocket] += chunkSize.size () + 4;
  sendHttpResponse (m_streamingSocket, data);
}

void CHTTPServer::clearStreaming ()
{
  if (m_httpsReply != nullptr)
  {
    m_httpsReply->deleteLater ();
    m_httpsReply = nullptr;
  }

  if (m_streamingSocket != nullptr)
  {
    m_streamingSocket->disconnectFromHost ();
    m_streamingSocket = nullptr;
  }
}

void CHTTPServer::abortStreaming ()
{
  if (m_httpsReply != nullptr)
  {
    m_httpsReply->abort ();
  }

  clearStreaming ();
}

