#include "datacaller.hpp"

USING_UPNP_NAMESPACE

CDataCaller::CDataCaller (QObject* parent) : QEventLoop (parent)
{
}

CDataCaller::~CDataCaller ()
{
}

QByteArray CDataCaller::callData (QUrl const & url, int timeout)
{
  QByteArray data; // Data returned
  if (!url.isEmpty () && !isRunning ())
  {
    m_request      = url.toString ();
    QString scheme = url.scheme ();
    if (scheme == "http" || scheme == "https")
    { // http or https request
      QNetworkAccessManager naMgr;
      QNetworkRequest       nreq (m_request); // Build the network request
      QNetworkReply*        reply = naMgr.get (nreq); // Send the get request and store the reply

      connect (reply, SIGNAL (error (QNetworkReply::NetworkError)), this, SLOT (error (QNetworkReply::NetworkError)));
      connect (reply, SIGNAL (finished ()), this, SLOT (finished ()));
      int  idTimer = startTimer (timeout); // Start the timeout timer to stop the event loop in case of timeout.
      bool success = exec (QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers) == 0;
      killTimer (idTimer); // The waiting event loop is finished. Kill the timer
      if (success)
      {
        data = reply->readAll (); // Get the reply
      }

      reply->deleteLater (); // Cleanup the reply during the application event loop
    }
  }

  return data;
}

// Timeout event.
void CDataCaller::timerEvent (QTimerEvent*)
{
  qDebug () << "CDataCaller::timerEvent: Quit on timeout";
  exit (-1);
}

// Launch by finished signal
void CDataCaller::finished ()
{
  exit (0);
}

void CDataCaller::error (QNetworkReply::NetworkError err)
{
  QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender ());
  QString        error = QString ("Network reply error:%1->%2->%3")
                       .arg (err).arg (reply->url ().toString ()).arg (reply->errorString ());
  qDebug () << "CDataCaller::error: " << err << " (" << error << ")";
  exit (err);
}
