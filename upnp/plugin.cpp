#include "plugin.hpp"
#include "waitingloop.hpp"
#include <QFileInfo>
#include <QBuffer>

USING_UPNP_NAMESPACE

CPlugin::CPlugin (CAuth::EType type)
{
  m_tokensTimer.setSingleShot (true);
  connect (&m_tokensTimer, &QTimer::timeout, this, &CPlugin::refreshTokenTimeout);
  if (type == CAuth::OAuth2)
  {
    m_auth = new COAuth2;
  }
}

CPlugin::~CPlugin ()
{
}

bool CPlugin::hasAutorisation ()
{
  bool     connected = false;
  COAuth2* auth      = static_cast<COAuth2*>(m_auth);
  if (auth != nullptr)
  {
    connected = !auth->value ("code").isEmpty ();
    if (!connected)
    {
      connected = auth->tryConnection ();
    }
  }

  return connected;
}

void CPlugin::setPixmap (QString const & fileName)
{
  m_pixmap = QPixmap (fileName);
}


COAuth2* CPlugin::oauth2 ()
{
  COAuth2* auth = nullptr;
  if (m_auth != nullptr && m_auth->type () == CAuth::OAuth2)
  {
    auth = static_cast<COAuth2*>(m_auth);;
  }

  return auth;
}

void CPlugin::startRefreshTokenTimer (int expiresIn)
{
  if (expiresIn > 0)
  {
    m_tokensTimer.start (expiresIn * 1000);
  }
}

void CPlugin::refreshTokenTimeout ()
{
  if (refreshToken ())
  {
    QString expiresIn = m_auth->value ("expires_in");
    startRefreshTokenTimer (expiresIn.toInt ());
  }
}

void CPlugin::callDataFinished ()
{
  exit (0);
}

void CPlugin::callDataError (QNetworkReply::NetworkError err)
{
  QNetworkReply* replySender = static_cast<QNetworkReply*>(sender ());
  QString        errorString = replySender->errorString ();
  qDebug () << "callDataError: " << err << " (" << errorString << ")";
  exit (-1);
}

QByteArray CPlugin::callData (EMethod method, QString const & request,
                              QList<TVarVal> const & headers, QByteArray const & data,
                              int timeout)
{
  QByteArray response;
  if (!request.isEmpty ())
  {
    bool            contentTypeHeader = false;
    QNetworkRequest nreq (request); // Build the network request
    nreq.setSslConfiguration (QSslConfiguration::defaultConfiguration ());
    foreach (TVarVal const & header, headers)
    {
      nreq.setRawHeader (header.first, header.second);
      if (header.first.toUpper () == "CONTENT-TYPE")
      {
        contentTypeHeader = true;
      }
    }

    if (!contentTypeHeader)
    {
      nreq.setHeader (QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    }

    if (m_nam == nullptr)
    {
      m_nam = new QNetworkAccessManager (this);
    }

    QNetworkReply* reply = nullptr;
    switch (method)
    {
      case Head :
        reply = m_nam->head (nreq); // Send the head request and store the reply
        break;

      case Post :
        reply = m_nam->post (nreq, data); // Send the post request and store the reply
        break;

      case Get :
        reply = m_nam->get (nreq); // Send the get request and store the reply
        break;

      case Put :
        reply = m_nam->put (nreq, data); // Send the put request and store the reply
        break;

      case Delete :
      {
        if (!data.isEmpty ())
        {
          if (m_deleteBuffer == nullptr)
          {
            m_deleteBuffer = new QBuffer (this);
          }

          m_deleteBuffer->setData (data);
          reply = m_nam->sendCustomRequest (nreq, "DELETE", m_deleteBuffer); // Send the delete request and store the reply
        }
        else
        {
          reply = m_nam->deleteResource (nreq);
        }
        break;
      }

      default :
        break;
    }

    if (reply != nullptr)
    {
      connect (reply, SIGNAL (error (QNetworkReply::NetworkError)), this, SLOT (callDataError (QNetworkReply::NetworkError)));
      connect (reply, SIGNAL (finished ()), this, SLOT (callDataFinished ()));
      // Launch the waiting event loop. The event loop is stopped by finished signal or timeout
      if (timeout == 0)
      {
        timeout = m_callDataTimeout;
      }

      int idTimer = startTimer (timeout);
      exec (QEventLoop::ExcludeUserInputEvents);
      killTimer (idTimer);
      response = reply->readAll ();
      reply->deleteLater (); // Cleanup the reply during the application event loop
    }
  }

  return response;
}

CBrowseReply CPlugin::browse (QString const & /*objectID*/, CContentDirectory::EBrowseType /*type*/, QString const & /*filter*/,
                              int /*startingIndex*/, int /*requestedCount*/, QString const & /*sortCriteria*/)
{
  return CBrowseReply ();
}





