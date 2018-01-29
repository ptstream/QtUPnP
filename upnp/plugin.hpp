#ifndef PLUGIN_HPP
#define PLUGIN_HPP

#include "contentdirectory.hpp"
#include "oauth2.hpp"
#include <QPixmap>
#include <QTimerEvent>
#include <QTimer>

class QBuffer;

START_DEFINE_UPNP_NAMESPACE

/*! \brief The base class for all media server plugins.
 *
 * A plugin is a wrapper between a none UPnP media server and the control point.
 * It must allow:
 * 1. An authentification method (optional).
 * 2. A method to fill CBrowseReply structure with the content of the server (mandatory).
 * 3. Provide an URL to read the media content using a QNetworkAccessManager (optional).
 * 4. Provide an HTTP URL for the renderer using HTTP server (optional).
 *
 * A plugin is similar at an UPnP media server. For this it provides at least:
 * 1. A name (mandatory).
 * 2. A friendly name (mandatory).
 * 3. An icon (optional).
 * 4. An UUID (mandatory).
 * 5. A browse function (mandatory).
 *
 * The plugin is restricted at media server but in the futur it is possible de generalize
 * the concept to any UPnP device (may be one day).
 * An example of code is googledrive plugin is in googledrive foler.
 */
class UPNP_API CPlugin : public QEventLoop
{
  Q_OBJECT
public:
  /*! Defines a pointer on a plugin. */
  typedef CPlugin* (*TFctPluginObject) ();

  /*! Defines a couple variable name-value. */
  typedef QPair<QByteArray, QByteArray> TVarVal;

  enum EMethod { Post, //!< Post method. For future use
                 Get, //!< Get method.
                 Put, //!< Put method. For future use
                 Delete, //!< Delete method. For future use
                 Head, //!< Head method.
               };

  /*! Constructor. */
  CPlugin (CAuth::EType type = CAuth::OAuth2);

  /*! Destructor. */
  virtual ~CPlugin ();

  /*! Returns the UUID. This identifier in a unique way the plugin */
  virtual QString uuid () const = 0;

  /*! Returns the friendly name. */
  virtual QString friendlyName () const = 0;

  /*! Returns true if the plugin has autorisation (connected).
   * Autorisation take more than one form. The application can use Autorisation to obtain permission
   * from users to read files. Actually only OAuth2.0 is implemented.
   */
  virtual bool hasAutorisation ();

  /*! Refresh the access token.
   * For example, in case og OAuth2 read files from remote servers needs a temporary token.
   * This token must be refresh periodically.
   */
  virtual bool refreshToken () { return false; }

  /*! Returns the pixmap. */
  virtual QPixmap pixmap () const { return m_pixmap; }

  /*! Returns the minimum QNetworkRequest to Get media data. */
  virtual QNetworkRequest mediaRequest (QString const &) { return QNetworkRequest (); }

  /*! Starts the refresh token timer. */
  void startRefreshTokenTimer (int expiresIn);

  /*! Returns the plugin name.
   * The plgin name is the file name of plugin (*.ids, *.png...).
   */
  QString const & name () const { return m_name; }

  /*! Sets the plugin name. */
  void setName (QString const & name) { m_name = name; }

  /*! Sets the pixmap. */
  void setPixmap (QString const & fileName);

  /*! Restores the autorisation from a file.
   * It is a short way to restore Autorisation.
   */
  void restoreAuth (QString const & fileName) { m_auth->restore (fileName); }

  /*! Save the autorisation to a file.
   * It is a short way to save Autorisation.
   */
  void saveAuth (QString const & fileName) const { m_auth->save (fileName); }

  /*! Returns the autorisation. */
  inline CAuth* auth ();

  /*! Returns the OAuth2 autorisation if it exists. */
  COAuth2* oauth2 ();

  /*! Returns the autorisation. */
  inline CAuth const * auth () const;

  /*! Returns the OAuth2 autorisation if it exists. */
  inline COAuth2 const * oauth2 () const;

  /*! Returns the server reply.
   * \param method: Get, Put, ...
   * \param request: The request to send.
   * \param headers: The headers to include in the request.
   * \param data: Data associated at the method (put, post, delete).
   * \param timeout: The timeout in ms.
   * \return The server reply.
   */
  QByteArray callData (EMethod method, QString const & request, QList<TVarVal> const & headers,
                       QByteArray const & data, int timeout = 0);

  /*! Returns the server reply.
   * \param method: Get, Put, ...
   * \param request: The request to send.
   * \param headers: The headers to include in the request.
   * \param timeout: The timeout in ms.
   * \return The server reply.
   */
  inline QByteArray callData (EMethod method, QString const & request,
                              QList<TVarVal> const & headers, int timeout = 0);

  /*! Returns the server reply.
   * \param method: Get, Put, ...
   * \param request: The request to send.
   * \param timeout: The timeout in ms.
   * \return The server reply.
   */
  inline QByteArray callData (EMethod method, QString const & request, int timeout = 0);

  /*! Returns the CBrowseReply.
   * \param objectID: Object identifier.
   * \param type: The browse type. Probably CContentDirectory::BrowseDirectChildren
   * \param filter: The reply filter.
   * \param startingIndex: The starting index. Probably 0.
   * \param requestedCount: The max number of elements in the reply.
   * \param sortCriteria: The sort criteria.
   * \return The broswe reply.
   */
  virtual CBrowseReply browse (QString const & /*objectID*/, CContentDirectory::EBrowseType /*type*/, QString const & /*filter*/,
                              int /*startingIndex*/, int /*requestedCount*/, QString const & /*sortCriteria*/) = 0;
protected slots :
  /*! The callData is finished. */
  void callDataFinished ();

  /*! The server has generated an error. */
  void callDataError (QNetworkReply::NetworkError err);

  /*! It is time to refresh the access token. */
  void refreshTokenTimeout ();

protected :
  QPixmap m_pixmap; //!< The pixmap.
  CAuth* m_auth = nullptr;//!< The autorisation.
  QNetworkAccessManager* m_nam = nullptr; //!< Network access manager.
  QBuffer* m_deleteBuffer = nullptr; //!< Futur use.
  int m_callDataTimeout = 20000; //!< The callData timeout.
  QTimer m_tokensTimer; //!< Timer to renew access token.
  QString m_name; //!< Plugin name e.g googledriver for Google Drive.
};

CAuth* CPlugin::auth ()
{
  return m_auth;
}

CAuth const * CPlugin::auth () const
{
  return m_auth;
}

COAuth2 const * CPlugin::oauth2 () const
{
  return const_cast<COAuth2 const *>(const_cast<CPlugin*>(this)->oauth2 ());
}

QByteArray CPlugin::callData (EMethod method, QString const & request,
                              QList<TVarVal> const & headers, int timeout)
{
  return callData (method, request, headers, QByteArray(), timeout);
}

QByteArray CPlugin::callData (EMethod method, QString const & request, int timeout)
{
  return callData (method, request, QList<TVarVal> (), QByteArray(), timeout);
}

} // Namespace

#endif // PLUGIN_HPP
