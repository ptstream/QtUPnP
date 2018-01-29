#include "httpserver.hpp"
#include "waitingloop.hpp"
#include "xmlhevent.hpp"
#include "helper.hpp"
#include <QTcpSocket>
#include <QDate>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QFileInfo>
#include <cstring>

USING_UPNP_NAMESPACE

//#define DUMP

#ifdef DUMP
static qint64 g_totalToStream = 0;
static qint64 g_streamed = 0;

static void dump (QByteArray data)
{
  qDebug () << "Send to renderer";
  int index            = data.indexOf ("\r\n\r\n");
  data                 = data.left (index);
  data.replace ("\r\n", "\n");
  QList<QByteArray> hs = data.split ('\n');
  for (QString const & h : hs)
  {
    qDebug () << h;
  }

  qDebug () << '\n';
}
#endif

CHTTPServer::CHTTPServer (QHostAddress const & address, quint16 port, QObject* parent) :
             QTcpServer (parent), m_done (false),
             m_httpsBufferSize (4096),
             m_httpsReadDataTimeout (1),
             m_httpsReadDataWaitingRetry (20000)
{
  bool success = listen (address, port);
  if (success)
  {
    m_httpsRequest.setSslConfiguration (QSslConfiguration::defaultConfiguration ());
    connect (this, &CHTTPServer::httpValidReadMessage, this, &CHTTPServer::sendResponse, Qt::QueuedConnection);
    connect (this, &CHTTPServer::streamingReady, this, &CHTTPServer::streamBlock, Qt::QueuedConnection);
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
  m_eventMessages[socket] = CHTTPParser (); // Add at the current map of sockets.
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
      CWaitingLoop connectToHostLoop (m_connectToHostTimeout, QEventLoop::ExcludeUserInputEvents);
      connect (socket, SIGNAL(connected()), &connectToHostLoop, SLOT(quit()));
      socket->connectToHost (socket->peerAddress (), socket->peerPort ());
      connectToHostLoop.exec (QEventLoop::ExcludeUserInputEvents);
      success = socket->state () == QAbstractSocket::ConnectedState;
      if (!success)
      {
        m_eventMessages.remove (socket);
        m_writingSocketSizes.remove (socket);
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
  emit rendererComStarted ();
  bool success = false;
  if (socket != nullptr)
  {
    success = connectToHost (socket);
    if (success)
    {
      qint64 written = socket->write (bytes, cBytes);
//      socket->waitForBytesWritten ();
      success = written > 0;
    }
  }

  emit rendererComEnded ();
  return success;
}

bool CHTTPServer::sendHttpResponse (QTcpSocket* socket, QByteArray const & bytes)
{
  return sendHttpResponse (socket, bytes.constData (), bytes.size ());
}

void CHTTPServer::sendResponse (CHTTPParser const * httpParser, QTcpSocket* socket)
{
  if (m_eventMessages.contains (socket))
  {
    QByteArray const & verb = httpParser->verb ();
    if (!verb.isEmpty ())
    {
      if (verb == "NOTIFY" &&
          httpParser->value ("NT") == "upnp:event" &&
          httpParser->value ("NTS") == "upnp:propchange")
      {
        m_vars.clear ();
        if (httpParser->contentLength () != 0)
        {
          QByteArray data = httpParser->message ().mid (httpParser->headerLength ());
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

          QByteArray sid = httpParser->sid (); // Save sid because httpParser must be deleted by accept.
          accept (socket);
          if (!m_vars.isEmpty ())
          {
            emit eventReady (sid);
          }
        }
        else
        {
          reject (socket);
        }
      }
      else
      {
        CHTTPParser::EQueryType type = httpParser->queryType ();
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
            QString request = httpParser->value (verb);
            emit mediaRequest (request);
            if (m_httpsRequest.url ().isValid ())
            {
              // Add significant headers to https request not in the request.
              QMap<QByteArray, QByteArray> const & headers = httpParser->headerElems ();
              for (QMap<QByteArray, QByteArray>::const_iterator it = headers.cbegin (), end = headers.cend (); it != end; ++it)
              {
                QByteArray const & name = it.key ();
                if (name != "HOST" && name != httpParser->verb () && !m_httpsRequest.hasRawHeader (name))
                {
                  m_httpsRequest.setRawHeader (name, it.value ());
                }
              }

              // Force the remote server to dowload raw data (e.g. not zip).
              m_httpsRequest.setRawHeader ("Accept-Encoding", "identity");
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
}

void CHTTPServer::streamBlock ()
{
  // Wait 20s max by default. In fact this code is to initialyze an event loop. Generally cWaitings < 3.
  int cWaitings = 0;
  while (m_httpsReply != nullptr && cWaitings < m_httpsReadDataWaitingRetry && m_httpsReply->bytesAvailable () == 0)
  {
    ++cWaitings;
    CWaitingLoop::wait (m_httpsReadDataTimeout, QEventLoop::AllEvents);
  }

  if (m_httpsReply != nullptr && cWaitings < m_httpsReadDataWaitingRetry)
  {
    emit serverComStarted ();
    QByteArray data = m_httpsReply->read (m_httpsBufferSize);
    emit serverComEnded ();
    sendHttpResponse (m_streamingSocket, data);
  }
}

void CHTTPServer::socketReadyRead ()
{
  QTcpSocket* socket = static_cast<QTcpSocket*>(sender ());
  if (m_eventMessages.contains (socket))
  {
    CHTTPParser* parser       = &m_eventMessages[socket];
    QByteArray&  eventMessage = parser->message ();
    while (socket->bytesAvailable () != 0)
    {
      eventMessage += socket->readAll ();
    }

    if (parser->parseMessage ())
    {
      emit httpValidReadMessage (parser, socket);
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

QByteArray CHTTPServer::headerResponse (qint64 contentLength, QByteArray const & contentType) const
{
  QByteArray const crlf = "\r\n";
  QByteArray header;
  header.reserve (1024);
  header  = "HTTP/1.1 200 OK" + crlf;
  header += "Server: UPnP/1.0 QtUPnP/1.0.0" + crlf;
  header += "Date: " + this->currentTime () + crlf;
  header += "Cache-control: no-cache" + crlf;
  header += "Content-Type: " + contentType + crlf;
  header += "Accept-Ranges: bytes" + crlf;
  header += "Content-Length: " + QByteArray::number (contentLength) + crlf;
  header += "TransferMode.dlna.org: Streaming" + crlf;
  header += "Connection: close" + crlf + crlf;
  return header;
}

void CHTTPServer::socketBytesWritten (qint64 bytes)
{
  QTcpSocket* socket = static_cast<QTcpSocket*>(sender ());
#ifdef DUMP
  if (socket == m_streamingSocket)
  {
    g_streamed += bytes;
    qDebug () << "Bytes written:" << bytes << ' ' << g_streamed << ' ' << nearestInt (100.0f * g_streamed / g_totalToStream) << '%';
  }
#endif

  // Only for known sockets.
  if (m_writingSocketSizes.contains (socket))
  {
    int   sizeToWrite = m_writingSocketSizes.value (socket); // Remains to stream.
    sizeToWrite      -= bytes;
    if (sizeToWrite == 0)
    { // Finished close the socket.
      socket->close ();
    }
    else
    {
      m_writingSocketSizes[socket] = sizeToWrite; // Store the new rest.
      if (socket == m_streamingSocket && socket->bytesToWrite () == 0)
      { // For streaming socket, if the renderer has dowloaded all data, ask a new block.
        emit streamingReady ();
      }
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
  }
}

void CHTTPServer::socketError (QAbstractSocket::SocketError err)
{
  QTcpSocket* socket = static_cast<QTcpSocket*>(sender ());
  qDebug () << "stocketError:" << socket << ' ' << err << ' ' << socket->errorString ();
  if (socket == m_streamingSocket)
  {
    abortStreaming ();
  }
}

bool CHTTPServer::startStreaming (QNetworkRequest const & request, QString const & method, QTcpSocket* socket)
{
  bool success = false;
  if (method == "GET" || method == "HEAD")
  { // Only get or head methods.
    if (m_naMgr == nullptr)
    {
      m_naMgr = new QNetworkAccessManager (this);
    }

    abortStreaming (); // Abort the current streaming and https download.
    m_streamingSocket = socket;
    if (method == "HEAD")
    {
      m_httpsReply = m_naMgr->head (request);
    }
    else
    {
      m_httpsReply = m_naMgr->get (request);
      m_httpsReply->setReadBufferSize (m_httpsBufferSize);
    }

    connect (m_httpsReply, static_cast<TFctNetworkReplyError>(&QNetworkReply::error), this, &CHTTPServer::httpsError);
    connect (m_httpsReply, &QNetworkReply::finished, this, &CHTTPServer::httpsFinished);
    connect (m_httpsReply, &QNetworkReply::readyRead, this, &CHTTPServer::httpsReadyRead);
    m_streamingResponseBuffer.clear ();
    m_readyReadConnected = true; // The slot readyRead is connected. See httpsFinished slot.
    success              = true;
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
  if (m_readyReadConnected)
  { // The https socket has emitted connect -> finished not connect -> readyRead -> finished
    emit m_httpsReply->readyRead ();
  }
  else
  { // Remaining data.
    qint64 bytesAvailable = m_httpsReply->bytesAvailable ();
    if (bytesAvailable != 0)
    { // Pending data if exists.
      emit serverComStarted ();
      QByteArray data = m_httpsReply->readAll ();
      emit serverComEnded ();
      sendHttpResponse (m_streamingSocket, data);
    }
  }

  m_httpsReply->deleteLater ();
  m_httpsReply = nullptr;
}

QByteArray CHTTPServer::streamingHeaderResponse (QNetworkReply const * reply, QList<QPair<QByteArray, QByteArray>> const & headers) const
{
  // Find if the name is in mandatory headers.
  auto hasHeader = [headers] (QByteArray const & name) -> bool
  {
    QList<QPair<QByteArray, QByteArray>>::const_iterator it  = headers.cbegin ();
    QList<QPair<QByteArray, QByteArray>>::const_iterator end = headers.cend ();
    for (; it != end && (*it).first.toUpper () != name.toUpper (); ++it);
    return it != end;
  };

  QByteArray const crlf = "\r\n";

  // Header tags to preseve.
  std::array<QByteArray, 10> const tags = { "DATE",
                                            "SERVER",
                                            "CONTENT-TYPE",
                                            "CONTENT-LENGTH",
                                            "CONTENT-RANGE",
                                            "CONTENT-ENCODING",
                                          };
  QByteArray header;
  header.reserve (1024);
  QVariant httpStatusCode   = reply->attribute (QNetworkRequest::HttpStatusCodeAttribute);
  QVariant httpReasonPhrase = reply->attribute (QNetworkRequest::HttpReasonPhraseAttribute);
  header                    = "HTTP/1.1 " + httpStatusCode.toByteArray () + ' ' +
                              httpReasonPhrase.toByteArray() + crlf;

  // Copy the important headers. Skip also mandatory headers.
  QList<QNetworkReply::RawHeaderPair> const & headerPairs = reply->rawHeaderPairs ();
  for (QNetworkReply::RawHeaderPair const & headerPair : headerPairs)
  {
    QByteArray                                name = headerPair.first.toUpper ();
    std::array<QByteArray, 7>::const_iterator end  = tags.cend ();
    if (std::find (tags.cbegin (), end, name) != end && !hasHeader (name))
    { // Header is in tags and not in mandatory.
      QByteArray value = headerPair.second;
      header          += name + ": " + value + crlf;
    }
  }

  // Adds mandatory headers.
  for (QPair<QByteArray, QByteArray> const & h : headers)
  {
    header += h.first + ": " + h.second + crlf;
  }

  // Add missing headers. Important for some renderers.
  header += "\
Cache-Control: private, max-age=0\r\n\
Accept-Ranges: bytes\r\n\
transferMode.dlna.org: Streaming\r\n\
Connection: close\r\n";
  header += crlf;
  return header;
}

void CHTTPServer::httpsReadyRead ()
{
  QNetworkReply* reply = static_cast<QNetworkReply*>(sender ());
  if (reply->isOpen ()) // In fact the reply is always open.
  {
    qint64 totalContentLength = reply->rawHeader ("content-length").toLongLong ();
#ifdef DUMP
    g_totalToStream = totalContentLength;
    g_streamed      = 0;
#endif
    QNetworkRequest req                = reply->request ();
    QByteArray      query              = req.rawHeader ("QtUPnP-Plugin-UrlQuery");

    // Extrac mandatory headers from the "QtUPnP-Plugin-UrlQuery" headers of the request.
    QList<QPair<QByteArray, QByteArray>> headers;
    if (!query.isEmpty ())
    {
      QList<QByteArray> values = query.split ('&'); // Split in form of "variable namen = value."
      QList<QByteArray> elem;
      for (QByteArray const & value : values)
      {
        elem = value.split ('='); // Split in 2 elements "name", "value".
        if (elem.size () == 2)
        { // Adds the pair of mandatory headers.
          headers.append (QPair<QByteArray, QByteArray> (elem[0].trimmed (), elem[1].trimmed ()));
        }
      }
    }

    m_streamingResponseBuffer = streamingHeaderResponse (reply, headers); // Prepare the header for renderers.
    int totalToWrite          = m_streamingResponseBuffer.size ();
    if (m_httpsReply->operation () == QNetworkAccessManager::GetOperation)
    { // For get operation, add the start of content data.
      emit serverComStarted ();
      m_streamingResponseBuffer += reply->read (m_httpsBufferSize);
      totalToWrite              += totalContentLength;
      emit serverComEnded ();
    }

    // Initialize the size to stream.
    m_writingSocketSizes.insert (m_streamingSocket, totalToWrite);

    // Disconnect the readyRead signal. It is the renderer who drive the https read.
    disconnect (reply, &QNetworkReply::readyRead, this, &CHTTPServer::httpsReadyRead);
    m_readyReadConnected = false;
#ifdef DUMP
    dump (m_streamingResponseBuffer);
#endif
    sendHttpResponse (m_streamingSocket, m_streamingResponseBuffer);
  }
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

