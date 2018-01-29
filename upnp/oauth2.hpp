#ifndef OAUTH2_HPP
#define OAUTH2_HPP

#include "xmlh.hpp"
#include <QEventLoop>
#include <QTimerEvent>
#include <QMap>

class QTcpServer;

START_DEFINE_UPNP_NAMESPACE

/*! \brief Defines an event loop to wait the end of connection.
 * For example, Google Drive needs to launch the internet browse to connect the application. */
class CAuthEventLoop : public QEventLoop
{
public:
  /*! Constructor. */
  CAuthEventLoop (QObject* parent = nullptr) : QEventLoop (parent) {}

  /*! Sets the timer id. See COAuth2::tryConnection. */
  void setTimer (int idTimer) { m_idTimer = idTimer; }

protected :
  /*! Event timeout to quit the event loop. */
  virtual void timerEvent (QTimerEvent* event)
  {
    if (event->timerId () == m_idTimer)
    {
      exit (1);
    }
  }

private :
  int m_idTimer = 0; //!< The timer identifier See COAuth2::tryConnection.
};

/*! \brief Base class of authentification process.
 *
 * Actually, only OAut2 is implemented. The identifier-paswword authentification will coded later.
 */
class UPNP_API CAuth : public QObject
{
public :
  /*! Authentification type. */
  enum EType { NoAuth, //!< No authentification.
               OAuth2 //!< OAuth2 authentification.
             };

  /*! Constructor. */
  CAuth (EType type, QObject* parent = nullptr) : QObject (parent), m_type (type) {}

  /*! Returns true if the connection is accepted. */
  virtual bool tryConnection () { return false; }

  /*! Restores the data from file. */
  virtual void restore (QString const &) {}

  /*! Saves the data to file. */
  virtual void save (QString const &) const {}

  /*! Gets values from authentification. For example see COAuth2. */
  virtual QString value (QString const &) const { return QString::null;  }

  /*! Returns the authentification type. */
  EType type () const { return m_type; }

  /*! The authentification is correctly initialized. */
  bool done () const { return m_done; }

protected :
  EType m_type = NoAuth; //!< The type
  bool m_done = false; //!< The initialization validity.
};

/*! \brief OAuth2 authentification process.
 *
 * For description of OAuth2.0 you can see https://oauth.net/2/.
 * Actually just "Authorization Code Grant" is implemented.
 * For security, all tokens are AES256 crypted and are decrypted just before use.
 * See also CControlPoint::LoadPlugins function.
 */
class COAuth2 : public CAuth, public CXmlH
{
  Q_OBJECT
public:
  /*! Constructor. */
  COAuth2 (QObject* parent = nullptr);

  /*! Sets the end point authentification.
   * E.g. for google drive "https://accounts.google.com/o/oauth2/v2/auth"
   */
  void setEndPoint (QString const & endPoint) { m_endPoint = endPoint; }

  /*! Sets the query variable names for authentification.
   * E.g. for google drive "scope", "response_type", "state", "redirect_uri", "client_id".
   */
  void setQuery (QStringList const & vars) { m_query = vars; }

  /*! Returns the query variable names for authentification. See above. */
  QStringList& query () { return m_query; }

  /*! Returns the query variable names and values for authentification. */
  QMap<QString, QString>& data () { return m_data; }

  /*! Adds a couple of variable name and value for authentification. */
  void addData (QString const & var, QString const & value) { m_data.insert (var, value); }

//  void updateQuery ();
  /*! Returns the access_token. */
  QString const & accessToken () const { return m_access_token; }

  /*! Returns true if the connection is accepted. */
  virtual bool tryConnection ();


  /*! Restores the data from file. E.g. for OAuth2, restore generally the client_id, client_secret,
   * access_token, refresh_token.
   */

  virtual void restore (QString const & fileName);
  /*! Saves the data to file. E.g. for OAuth2, restore generally the client_id, client_secret,
   * access_token, refresh_token.
   */
  virtual void save (QString const & fileName) const;

  /*! Uses for xml autentification file. */
  virtual bool characters (QString const & name);

  /*! Returns the value of variable name. */
  virtual QString value (QString const & var) const;

protected slots :
  /*! The redirect uri server receive a new connection. */
  void newConnection ();

  /*! The remote server sends data for an authentification . */
  void readyRead ();

  /*! The remote server was disconnected. */
  void disconnected ();

signals :
  /*! The signal will be emitted at the end of authentification. */
  void authorizationFinish ();

private :
  /*! Sends a simple html message at the end of connection. */
  QByteArray htmlConnectionResponse (QString const & text, bool ok);

private :
  QMap<QString, QString> m_data; //!< Map of couple variable names-values.
  QStringList m_query; //!< List of variables used by authentification (e.g. "access_token").
  QString m_endPoint; //!< The authentification edn point e.g. for google drive "https://accounts.google.com/o/oauth2/v2/auth"
  QString m_redirectUri = "http://localhost:8080/aivctrl-oauth-callback"; //!< Redirect uri for OAuth2
  QTcpServer* m_server = nullptr; //!< The server for redirect uri.

  QString const m_client_id = "client_id"; //!< Predifined key
  QString const m_client_secret = "client_secret"; //!< Predifined key
  QString const m_access_token = "access_token"; //!< Predifined key
  QString const m_refresh_token = "refresh_token"; //!< Predifined key
};

} // Namespace

#endif // OAUTH2_HPP
