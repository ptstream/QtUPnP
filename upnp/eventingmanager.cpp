#include "eventingmanager.hpp"
#include "helper.hpp"
#include "dump.hpp"
#include <QHostAddress>
#include <QElapsedTimer>
USING_UPNP_NAMESPACE

int CEventingManager::m_elapsedTime = 0;
QString CEventingManager::m_lastError;

CEventingManager::CEventingManager () : m_naMgr (new QNetworkAccessManager (this))
{
}

CEventingManager::CEventingManager (QNetworkAccessManager* naMgr) : m_naMgr (naMgr)
{
  if (m_naMgr == nullptr)
  {
    m_naMgr = new QNetworkAccessManager (this);
  }
}

void CEventingManager::timerEvent (QTimerEvent*)
{
  qDebug () << "CEventingManager::timerEvent: Quit on timeout";
  exit (Timeout);
}

void CEventingManager::finished ()
{
  exit (NoError);
}

void CEventingManager::error (QNetworkReply::NetworkError err)
{
  auto    replySender = dynamic_cast<QNetworkReply*>(sender ());
  QString errorString = replySender->errorString ();
  qDebug () << "CEventingManager::error: " << err << " (" << errorString << ")";
  exit (err);
}

bool CEventingManager::sendRequest (QUrl url, char const * verb, QString const & eventSubURL,
                                    QNetworkRequest& req, int requestTimeout)
{
  url.setPath (eventSubURL);
  req.setUrl (url);

  QElapsedTimer time;
  time.start ();

  QNetworkReply* reply = m_naMgr->sendCustomRequest (req, verb);
  connect (reply, &QNetworkReply::errorOccurred, this, &CEventingManager::error);
  connect (reply, SIGNAL(finished()), this, SLOT(finished()));

  int  idTimer = startTimer (requestTimeout);
  bool success = exec (QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers) == NoError;
  killTimer (idTimer);
  if (success)
  {
    m_sid = reply->rawHeader ("SID"); // Valid only for verb="SUBSCRIBE".
  }
  else
  {
    int statusCode = reply->attribute (QNetworkRequest::HttpStatusCodeAttribute).toInt ();
    m_lastError    = reply->attribute (QNetworkRequest::HttpReasonPhraseAttribute).toString ();
    qDebug () << verb << " failed. Response from the server:" << statusCode
              << m_lastError << " from:" << url;
    m_lastError.prepend (QString ("(%1) ").arg (statusCode));
    QString text ("CEventingManager::sendRequest:");
    text += verb;
    text += ':';
    text += url.toString () + ':' + m_lastError;
    CDump::dump (text);
  }

  reply->deleteLater ();
  m_elapsedTime = time.elapsed ();
  return success;
}

bool CEventingManager::subscribe (QUrl const & url, QString const & eventSubURL,
                                  QHostAddress const & serverAddress,
                                  quint16 serverPort, int renewalDelay, int requestTimeout)
{
  QNetworkRequest req;

  QByteArray value ("Second-");
  value += QByteArray::number (renewalDelay);
  req.setRawHeader ("TIMEOUT", value);

  value = buildSystemHeader ().toUtf8 () +
          " UPnP/1.1 " +
          libraryName () +
          '/' +
          libraryVersion ();
  req.setHeader (QNetworkRequest::UserAgentHeader, value);

  value = "<http://" +
          serverAddress.toString ().toUtf8 () +
          ':' +
          QByteArray::number (serverPort) +
          "/event>";
  req.setRawHeader ("CALLBACK", value);
  req.setRawHeader ("NT", "upnp:event");
  bool success = sendRequest (url, "SUBSCRIBE", eventSubURL, req, requestTimeout);
  if (success)
  {
    success      = !m_sid.isEmpty ();
    m_renewDelay = renewalDelay;
  }

  return success;
}

bool CEventingManager::renewSubscribe (QUrl const & url, QString const & eventSubURL,
                                       QString const & sid, int requestTimeout)
{
  QNetworkRequest req;

  QByteArray value ("Second-");
  value += QByteArray::number (m_renewDelay);
  req.setRawHeader ("TIMEOUT", value);
  req.setRawHeader ("SID", sid.toUtf8 ());
  return sendRequest (url, "SUBSCRIBE", eventSubURL, req, requestTimeout);
}

bool CEventingManager::unsubscribe (QUrl const & url, QString const & eventSubURL,
                                    QString const & sid, int requestTimeout)
{
  QNetworkRequest req (url);
  req.setRawHeader ("SID", sid.toUtf8 ());
  return sendRequest (url, "UNSUBSCRIBE", eventSubURL, req, requestTimeout);
}
