#ifndef PLUGINOBJECT_HPP
#define PLUGINOBJECT_HPP

#include "googledrive_global.hpp"
#include "../upnp/plugin.hpp"

/*! \brief This class defines the caracteristics of the google drive plugin see QtUPnP::CPlgin class.
 * To use this plugin in your application you must register your application.
 * For this, see https://developers.google.com/identity/protocols/OAuth2.
 */
class CPluginObject : public QtUPnP::CPlugin
{
public:
  CPluginObject ();
  virtual ~CPluginObject ();

  /*! Returns the plugin uuid. */
  virtual QString uuid () const { return m_uuid; }

  /*! Returns the friendly name. */
  virtual QString friendlyName () const { return m_friendlyName; }

  /*! Returns true if the plugin OAuth2 es valid. */
  virtual bool isValidOAuth2Autorisation ();

  /*! The OAuth2 refresh token as expired. Refresh it. */
  virtual bool refreshToken ();

  /*! Builds the media request to get media informations (generally from CDidlItem identifer). */
  virtual QNetworkRequest mediaRequest (QString const & id);

  /*! Browse the native hierarchy of the Google backup&sync (Google Drive).
   * \param objectID: The identifier of a folder.
   * \param type: No implemented. This corresponds to the UPnP BrowseFlag "BrowseDirectChildren”.
   * \param startingIndex: Not implemented actually. Google uses a string not an integer.
   * \param requestedCount: Not implemented actually.
   * \param sortCriteria: Only +dc:title or -dc:title is implemented. By default it is +dc:title.
   * \return The list of items (QtUPnP::CBrowseReply).
   */
  virtual QtUPnP::CBrowseReply browse (QString const & objectID, QtUPnP::CContentDirectory::EBrowseType type,
               QString const & filter, int startingIndex, int requestedCount,
               QString const & sortCriteria);

private :
  /*! Adds to http query the variable with its value.
   * The separator must be '?' for the first variable.
   */
  void addToQuery (QString& query, QString const & var, QString const & value, QChar const separator = QLatin1Char ('&'));

  /*! Adds to http query the variable with the corresponding oauth2 value.
   * The separator must be '?' for the first variable.
   */
  void addToQuery (QString& query, QString const & var, QChar const separator = QLatin1Char ('&'));

  /*! Adds to http query the access token.
   * It is addToQuery (query, "access_token, separator);
   * The separator must be '?' for the first variable.
   */
  void addAccessToken (QString& query, QChar const separator = QLatin1Char ('&'));

  /*! Returns the access token to add at a request. */
  TVarVal accessTokenHeader () const;

  /*! Verify if current access_token is valid. */
  bool updateAccessToken ();

public :
  static CPluginObject* m_plugicObject; //!< The global plugin.

private :
  static QString const m_name; //!< The name of the plugin.
  static QString const m_friendlyName; //!< The friendly name of the plugin.
  static QString const m_uuid; //!< The uuid of the plugin.
  static QString const m_apiHost; //!< "https://www.googleapis.com/drive/v3/"
};

/*! Returns the global plugin structure. */
extern "C" GOOGLEDRIVESHARED_EXPORT QtUPnP::CPlugin* pluginObject ();

#endif // PLUGINOBJECT_HPP
