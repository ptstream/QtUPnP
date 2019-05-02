#include "pluginobject.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileInfo>

USING_UPNP_NAMESPACE

CPluginObject* CPluginObject::m_plugicObject = nullptr;
QString const CPluginObject::m_name = "googledrive";
QString const CPluginObject::m_friendlyName = "Google Drive (Beta)";
QString const CPluginObject::m_uuid = "uuid:6a9ab8a8-f496-451b-8a86-1a146ed92aac";
QString const CPluginObject::m_apiHost = "https://www.googleapis.com/drive/v3/";

CPluginObject::CPluginObject () : CPlugin (CAuth::OAuth2)
{
  COAuth2* auth = this->oauth2 ();
  auth->addData ("scope", "openid%20profile%20https://www.googleapis.com/auth/drive.readonly");
  auth->addData ("response_type", "code");
  auth->addData ("state", QString ());
  auth->addData ("redirect_uri", QString ());
  auth->setEndPoint ("https://accounts.google.com/o/oauth2/v2/auth");
  auth->setQuery (QStringList ({ "scope", "response_type", "state", "redirect_uri", "client_id" }));
}

CPluginObject::~CPluginObject ()
{
}

QtUPnP::CPlugin* pluginObject ()
{
  if (CPluginObject::m_plugicObject == nullptr)
  {
    CPluginObject::m_plugicObject = new CPluginObject;
  }

  return CPluginObject::m_plugicObject;
}

CPlugin::TVarVal CPluginObject::accessTokenHeader () const
{
  COAuth2 const *  auth = oauth2 ();
  QByteArray headerValue ("Bearer ");
  headerValue += auth->value (auth->accessToken ());
  return CPlugin::TVarVal ("Authorization", headerValue);
}

void CPluginObject::addToQuery (QString& query, QString const & var, QString const & value, QChar const & separator)
{
  query += separator + var + '=' + value;
}

void CPluginObject::addToQuery (QString& query, QString const & var, QChar const & separator)
{
  addToQuery (query, var, oauth2 ()->value (var), separator);
}

void CPluginObject::addAccessToken (QString& query, QChar const & separator)
{
  addToQuery (query, "access_token", separator);
}

bool CPluginObject::isValidOAuth2Autorisation ()
{
  bool connected = updateAccessToken (); // Verify if current access_token is valid.
  if (!connected)
  { // The access_token is not valid or has expired.
    connected = CPlugin::hasAutorisation (); // Send a request to Google's OAuth 2.0 server.
    if (connected)
    { // The user accept the connection. Get OAuth 2.0 access_token and refresh_token.
      COAuth2* auth = oauth2 ();
      QString query ("https://www.googleapis.com/oauth2/v4/token");
      addToQuery (query, "code", '?');
      addToQuery (query, "client_id");
      addToQuery (query, "client_secret");
      addToQuery (query, "redirect_uri");
      addToQuery (query, "grant_type", "authorization_code");
      QByteArray data = callData (CPlugin::Post, query);
      if (!data.isEmpty () )
      {
        QJsonParseError     error;
        QJsonDocument       doc (QJsonDocument::fromJson (data, &error));
        QJsonObject const & json = doc.object ();
        connected                = error.error == QJsonParseError::NoError;
        if (connected)
        { // Parse the return.
          QString  accessToken = json["access_token"].toString ();
          if (!accessToken.isEmpty ())
          {
            auth->addData ("access_token", accessToken);
            auth->addData ("refresh_token", json["refresh_token"].toString ());
            int expiresIn = json["expires_in"].toInt();
            auth->addData ("expires_in", QString::number(expiresIn));
            startRefreshTokenTimer (expiresIn); // Start the refresh token timer.
          }
        }
      }
    }
  }

  return connected;
}

bool CPluginObject::refreshToken ()
{
  bool    success = false;
  QString query ("https://www.googleapis.com/oauth2/v4/token");
  addToQuery (query, "refresh_token", '?');
  addToQuery (query, "client_id", '&');
  addToQuery (query, "client_secret", '&');
  addToQuery (query, "grant_type", "refresh_token");
  QByteArray data = callData (CPlugin::Post, query);
  if (!data.isEmpty ())
  {
    QJsonParseError     error;
    QJsonDocument       doc (QJsonDocument::fromJson (data, &error));
    QJsonObject const & json = doc.object ();
    success                  = error.error == QJsonParseError::NoError;
    if (success)
    {
      QString  accessToken = json["access_token"].toString ();
      success              = !accessToken.isEmpty ();
      if (success)
      {
        COAuth2* auth = this->oauth2 ();
        auth->addData ("access_token", accessToken);
        auth->addData ("expires_in", json["expires_in"].toString ());
      }
    }
  }

  return success;
}

 // Verify if current access_token is valid.
bool CPluginObject::updateAccessToken ()
{
  COAuth2* auth        = this->oauth2 ();
  bool     success     = false;
  QString  accessToken = auth->value ("access_token");
  if (!accessToken.isEmpty ())
  {
    QString    query ("https://www.googleapis.com/oauth2/v3/tokeninfo?access_token=");
    query          += accessToken;
    QByteArray data = callData (CPlugin::Get, query);
    if (!data.isEmpty ())
    {
      QJsonParseError     error;
      QJsonDocument       doc (QJsonDocument::fromJson (data, &error));
      QJsonObject const & json = doc.object ();
      success                  = error.error == QJsonParseError::NoError;
      if (success)
      {
        QString  clientID = json["aud"].toString ();
        success           = !clientID.isEmpty ();
        if (success)
        {
          success = clientID == auth->value ("client_id");
          if (success)
          {
            QString expiresIn = json["expires_in"].toString ();
            auth->addData ("expires_in", expiresIn);
          }
        }
      }

      if (!success && !auth->accessToken ().isEmpty ())
      {
        success = refreshToken ();
      }

      if (success)
      {
        QString expiresIn = m_auth->value ("expires_in");
        startRefreshTokenTimer (expiresIn.toInt ());
      }
    }
  }

  return success;
}

QNetworkRequest CPluginObject::mediaRequest (QString const & id)
{
  QNetworkRequest request;
  if (!id.isEmpty ())
  {
    QByteArray query;
    QString    googleID = QUrl::fromPercentEncoding (id.toUtf8 ());
    int        index    = googleID.indexOf ('?');
    if (index != -1)
    {
      query    = googleID.mid (index + 1).toUtf8 ();
      googleID = googleID.left (index);
    }

    QString url = m_apiHost + "files/" + googleID + "?alt=media";
    request.setUrl (url);
    request.setHeader (QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    CPlugin::TVarVal accessToken = CPluginObject::accessTokenHeader ();
    request.setRawHeader (accessToken.first, accessToken.second);
    if (!query.isEmpty ())
    {
      request.setRawHeader ("QtUPnP-Plugin-UrlQuery", query);
    }
  }

  return request;
}
