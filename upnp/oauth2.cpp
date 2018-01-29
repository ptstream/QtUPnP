#include "oauth2.hpp"
#include "aesencryption.h"
#include <QUrl>
#include <QDesktopServices>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDateTime>
#include <QCryptographicHash>
#include <QXmlStreamWriter>

USING_UPNP_NAMESPACE

COAuth2::COAuth2 (QObject* parent) : CAuth (OAuth2, parent), m_server (new QTcpServer (this))
{
  m_done = m_server->listen (QHostAddress::Any, 8080);
  if (m_done)
  {
    connect (m_server, &QTcpServer::newConnection, this, &COAuth2::newConnection);
  }
}

bool COAuth2::tryConnection ()
{
  bool success = true;
  if (m_data.value ("code").isEmpty ())
  { // The code is empty. Get it.
    QString               query;
    QStringList::iterator begin = m_query.begin (), end = m_query.end ();
    for (QStringList::iterator it = begin; it != end; ++it)
    {
      QString const & var   = *it;
      QString         value = m_data.value (var);
      if (var == "state" && value.isEmpty ())
      {
        qsrand (QDateTime::currentDateTime ().toTime_t ());
        quint32 state = (static_cast<quint32>(qrand ()) & 0x7fff) | ((static_cast<quint32>(qrand ()) & 0x7fff) << 16);
        value         = QString::number (state);
        m_data[var]   = value;
      }
      else if (var == "redirect_uri" && value.isEmpty ())
      {
        value       = m_redirectUri;
        m_data[var] = value;
      }

      if (it != begin)
      {
        query += '&';
      }

      query += var + '=' + value;
    }

    QUrl url (m_endPoint, QUrl::StrictMode); // Create the url.
    url.setQuery (query, QUrl::StrictMode); // Encode the query.

#ifdef Q_OS_LINUX
     // For Kubuntu, for example, QDesktopServices::openUrl try to launch kde-open5 but kde-open5 needs, may be, Qt with different version.
     // Probably the problem is QtCreator defines LD_LIBRARY_PATH with the current version of Qt.
     // To solve this, I choose to launch directly the current browser with the system function.
    QByteArray command ("x-www-browser \"");
    command += url.toString ().toLatin1 () + '"';
    success = system (command.constData ()) != -1;
#else
    success = QDesktopServices::openUrl (url); // Launch the system browser
#endif
    if (success)
    { // Wait for browser response. The response is redirected to the authentification server.
      CAuthEventLoop loop;
      int            idTimer = loop.startTimer (20000); // Wait 20s maximum.
      loop.setTimer (idTimer);
      connect (this, &COAuth2::authorizationFinish, &loop, &CAuthEventLoop::quit);
      success = loop.exec (QEventLoop::ExcludeUserInputEvents) == 0;
      loop.killTimer (idTimer);
    }
  }

  return success;
}

void COAuth2::newConnection ()
{
  QTcpSocket* socket = m_server->nextPendingConnection ();
  connect (socket, &QTcpSocket::readyRead, this, &COAuth2::readyRead);
  connect (socket, &QTcpSocket::disconnected, this, &COAuth2::disconnected);
}

QByteArray COAuth2::htmlConnectionResponse (QString const & text, bool ok)
{
  QString content = QString ("\
<!DOCTYPE html>\
<html>\
<head>\
<h2 align=\"center\">%1</h2>\
</title>\
</head>\
<body>\
<h3 align=\"center\">\
<font color=\"%2\">%3</font>\
</h3>\
<p style=\"text-align:center\"><a href=\"#\" onClick=\"self.close();\">%4<SUP>*</SUP></a></p>\
<p style=\"text-align:center\"><sup>*</sup>%5</p>\
</body>\
</html>").arg (tr ("Connection"))
         .arg (ok ? "green" : "red")
         .arg (text)
         .arg (tr ("You can close your browser"))
         .arg (tr ("Due to the browser security policy, this button may not work. Use standard close application."));

  return content.toUtf8 ();
}

void COAuth2::readyRead ()
{
  bool        done   = false;
  QTcpSocket* socket = static_cast<QTcpSocket*>(sender ());
  QByteArray  data   = socket->readAll ();
  if (data.startsWith ("GET /aivctrl-oauth-callback"))
  {
    int index = data.indexOf ("\r\n");
    if (index != -1)
    {
      data.truncate (index);
      QList<QByteArray> elems = data.split ('&');
      if (!elems.isEmpty ())
      {
        QList<QByteArray> elem0s = elems.first ().split ('?');
        if (elem0s.size () == 2)
        {
          elems[0] = elem0s[1];
        }

        for (QList<QByteArray>::const_iterator it = elems.cbegin (), end = elems.cend (); it != end; ++it)
        {
          QList<QByteArray> elem = (*it).split ('=');
          if (elem.size () == 2)
          {
            QString var = QString::fromUtf8 (elem[0]);
            QString val = QString::fromUtf8 (elem[1]);
            if (var == "state" && m_data.contains (var))
            {
              if (val != m_data.value (var))
              {
                m_data.remove ("code");
                break;
              }
            }

            m_data.insert (var, val);
          }
        }
      }
    }

    done = !m_data.value ("code").isEmpty ();
  }

  QString text;
  if (done)
  {
    text = tr ("Connected.");
  }
  else
  {
    text = tr ("Connection fails.");
  }

  QString    content  = htmlConnectionResponse (text, done);
  QByteArray response ("HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: ");
  response += QByteArray::number (content.size ()) + "\r\n\r\n";
  response += content.toUtf8 ();
  socket->write (response);
  emit authorizationFinish ();
}

void COAuth2::disconnected ()
{
  QTcpSocket* socket = static_cast<QTcpSocket*>(sender ());
  socket->deleteLater ();
}

bool COAuth2::characters (QString const & name)
{
  QString         var;
  QString const & tag = m_stack.top ();
  if (tag == "id0")
  {
    var = m_data.value (m_client_id).isEmpty () ? m_client_id : m_access_token;
  }
  else if (tag == "id1")
  {
    var = m_data.value (m_client_secret).isEmpty () ? m_client_secret : m_refresh_token;
  }

  if (!var.isEmpty ())
  {
#if HIDDENFILES==1
// The AIVCtrl AES256 encoded in base 64 key and iv of your application
#include "../../Hidden-Files/aes256.h"
#else
// The example AES256 key and iv to build from GitHub.
#include "aes256.h"
#endif
    QByteArray value   = QByteArray::fromBase64 (name.toLatin1 ());
    QByteArray hashKey = QCryptographicHash::hash (key, QCryptographicHash::Sha256);
    QByteArray hashIV  = QCryptographicHash::hash (iv, QCryptographicHash::Sha256);
    value              = CAESEncryption::Decrypt (CAESEncryption::AES_256, CAESEncryption::CBC, value, hashKey, hashIV);
    m_data[var]        = value;
  }

  return true;
}

QString COAuth2::value (QString const & var) const
{
  QString value = m_data.value (var);
  return value;
}

/* Plugin files.
 * 1. /application folder/plugins/plugin name.ids
 *    e.g. %ProgramFiles(x86)%/aivctr/plugins/googledrive.ids for Google Drive).
 *    id0 = AES256 encryted client_id
 *    id1 = AES256 encryted client_secret
 *
 * 2. /application data/aivctrl/plugin name.ids
 *    e.g. %APPDATA%/aivctr/googledrive.ids for Google Drive).
 *    id0 = AES256 encryted access_token
 *    id1 = AES256 encryted refresh_token
 */

void COAuth2::restore (QString const & fileName)
{
  QByteArray content;
  QFile file (fileName);
  if (file.open (QIODevice::ReadOnly))
  {
    content = file.readAll ();
    file.close ();
  }

  parse (content);
}

void COAuth2::save (QString const & fileName) const
{
  if (!m_data.value (m_access_token).isEmpty ())
  {
    QFile file (fileName);
    if (file.open (QIODevice::WriteOnly | QIODevice::Text))
    {
      QXmlStreamWriter stream (&file);
      stream.setAutoFormatting (true);
      stream.writeStartDocument ();

      stream.writeStartElement ("plugin");
      stream.writeAttribute ("version", "1.0.0");

      QVector<QPair<QString, QString>> elems = { QPair<QString, QString> ("id0", m_access_token),
                                                 QPair<QString, QString> ("id1", m_refresh_token)
                                               };
      for (QPair<QString, QString> const & elem : elems)
      {
        stream.writeStartElement (elem.first);
        QByteArray token   = m_data.value (elem.second).toLatin1 ();
#if HIDDENFILES==1
// The AIVCtrl AES256 key and iv
#include "../../Hidden-Files/aes256.h"
#else
// The example AES256 key and iv to build from GitHub.
#include "aes256.h"
#endif
        QByteArray hashKey = QCryptographicHash::hash (key, QCryptographicHash::Sha256);
        QByteArray hashIV  = QCryptographicHash::hash (iv, QCryptographicHash::Sha256);
        token              = CAESEncryption::Crypt (CAESEncryption::AES_256, CAESEncryption::CBC, token, hashKey, hashIV);
        token              = token.toBase64 ();
        stream.writeCharacters (token);
        stream.writeEndElement ();
      }

      stream.writeEndDocument ();
    }
  }
}
