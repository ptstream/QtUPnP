
#include "actionmanager.hpp"
#include "actioninfo.hpp"

USING_UPNP_NAMESPACE

int CActionManager::m_elapsedTime = 0;
QString CActionManager::m_lastError;

CActionManager::CActionManager (QObject* parent ) : QEventLoop (parent)
{
}

CActionManager::~CActionManager ()
{
}

void CActionManager::timerEvent (QTimerEvent*)
{
  qDebug () << "CActionManager::timerEvent: Quit on timeout";
  exit (-1);
}

void CActionManager::finished ()
{
  exit (0);
}

void CActionManager::error (QNetworkReply::NetworkError err)
{
  QNetworkReply* replySender = static_cast<QNetworkReply*>(sender ());
  QString        errorString = replySender->errorString ();
  qDebug () << "CActionManager::error: " << err << " (" << errorString << ")";
  emit networkError (m_device, err, errorString);
}

bool CActionManager::post (QString const & device, QUrl const & url,
                           CActionInfo& info, int timeout)
{
  m_lastError.clear ();
  m_device     = device;
  bool success = false;
  if (!isRunning ())
  {
    QNetworkRequest req (url);
    req.setHeader (QNetworkRequest::ContentTypeHeader, QString ("text/xml; charset=\"utf-8\""));

    // Build "[sevice type]#[action name]". e.g. "urn:schemas-upnp-org:service:AVTransport:1#Stop"
    QString const & actionName = info.actionName ();
    QString soapActionHdr = QString ("\"%1#%2\"").arg (info.serviceID ()).arg (actionName);
    req.setRawHeader ("SOAPAction", soapActionHdr.toUtf8 ());

    QNetworkAccessManager naMgr;

    QTime time;
    time.start ();

    QNetworkReply* reply = naMgr.post (req, info.message ().toUtf8 ());
    connect (reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
    connect (reply, SIGNAL(finished()), this, SLOT(finished()));

    int idTimer = startTimer (timeout);
    success     = exec (QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers) == 0;
    killTimer (idTimer);
    if (success)
    {
      info.setResponse (reply->readAll ());
    }
    else
    {
      qint32 statusCode = reply->attribute (QNetworkRequest::HttpStatusCodeAttribute).toInt ();
      m_lastError       = reply->attribute (QNetworkRequest::HttpReasonPhraseAttribute).toString ();
      QString message   = QString ("Action failed. Response from the server: %1, %2").arg (statusCode).arg (m_lastError);
      message          += '\n';
      message          += url.toString () + '\n';
      message          += info.message () + "\n\n";
      qDebug () << "CActionManager::post:" << message;
      m_lastError.prepend (QString ("(%1) ").arg (statusCode));
    }

    reply->deleteLater ();
    m_elapsedTime = time.elapsed ();
  }

  return success;
}
