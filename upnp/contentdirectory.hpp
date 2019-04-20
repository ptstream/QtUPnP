#ifndef CONTENT_DIRECTORY_HPP
#define CONTENT_DIRECTORY_HPP

#include "control.hpp"
#include "controlpoint.hpp"
#include "browsereply.hpp"

START_DEFINE_UPNP_NAMESPACE

/*! \brief A wrapper that manages the
 * urn:schemas-upnp-org:service:ContentDirectory:1 service actions.
 */
class UPNP_API CContentDirectory : public CControl
{
public:
  /*! The browse types. */
  enum EBrowseType {
                     BrowseMetaData, //!< Return from metadata.
                     BrowseDirectChildren, //!< Return the children.
                   };

  /*! The defaut constructor. */
  CContentDirectory () {}

  /*! The constructor from a control point.
   * \param cp: The control point address.
   */
  CContentDirectory (CControlPoint* cp)  : CControl (cp) {}

  /*! Set the browse action tiemout.
   * \param timeout: The delay in ms. By default 20s.
   */
  void setBrowseTimeout (int timeout) { m_browseTimeout = timeout; }

  /*! Return the browse action timeout. */
  int browseTimeout () const { return m_browseTimeout; }

  /*! Browses from an identifier.
   * \param serverUUID: Server uuid.
   * \param objectID: Upnp identifier.
   * \param type: Type of browse. Must be BrowseMetaData or BrowseDirectChildren.
   * \param filter: Filter of the request. For syntax see http://upnp.org/specs/av/UPnP-av-ContentDirectory-Service.pdf.
   *                The filter capabilites is returned by GetSearchCapabilities action.
   * \param startingIndex: The first index returned.
   * \param requestedCount: The max number of resquests returned. 0 indicates all is rteurned.
   * \param sortCriteria: The sort criteria. For syntax see http://upnp.org/specs/av/UPnP-av-ContentDirectory-Service.pdf.
   *                      The sort criteria is returned by GetSortCapabilities action.
   * \return The decoded result.
   */
   CBrowseReply browse (QString const & serverUUID, QString const & objectID,
                       EBrowseType type = BrowseDirectChildren, QString const & filter = "*",
                       int startingIndex = 0, int requestedCount = 0,
                       QString const & sortCriteria =  QString ());

  /*! Searchs from a container identifier.
   * \param serverUUID: Server uuid.
   * \param containerID: Server uuid.
   * \param searchCriteria: Search criteria of the request. For syntax see http://upnp.org/specs/av/UPnP-av-ContentDirectory-Service.pdf.
   *                        The filter capabilites is returned by GetSearchCapabilities action.
   * \param filter: Filter of the request. For syntax see http://upnp.org/specs/av/UPnP-av-ContentDirectory-Service.pdf.
   *                The filter capabilites is returned by GetSearchCapabilities action.*
   * \param startingIndex: The first index returned.
   * \param requestedCount: The max number of resquests returned. 0 indicates all is rteurned.
   * \param sortCriteria: The sort criteria. For syntax see http://upnp.org/specs/av/UPnP-av-ContentDirectory-Service.pdf.
   *                      The sort criteria is returned by GetSortCapabilities action.
   * \return The decoded result.
   */
  CBrowseReply search (QString const & serverUUID, QString const & containerID,
                       QString const & searchCriteria, QString const & filter = "*",
                       int startingIndex = 0, int requestedCount = 0,
                       QString const & sortCriteria =  QString ());

  /*! Returns search capabilities.
   * \param serverUUID: Server uuid.
   * \return The search capabilities.
   */
  QStringList getSearchCaps (QString const & serverUUID);

  /*! Returns sort capabilities.
   * Service urn:schemas-upnp-org:service:ContentDirectory.
   * \param serverUUID: Server uuid.
   * \return The sort capabilities.
   */
  QStringList getSortCaps (QString const & serverUUID);

  /*! Returns the system update ID.
   * \return Current system ID.
   */
  unsigned getSystemUpdateID (QString const & serverUUID);

  /*! Returns true if the item is valid.
   * \param item: CDidlItem to check.
   * \return True if item is valid.
   */
  bool isValidItem (CDidlItem const & item);

  /*! Returns the indices of invalid items.
   * \param items: CDidlItem to check.
   * \return The indices of invalid items.
   */
  QList<int> invalidItems (QList<CDidlItem> const & items);

private :
  /*! Prepares the arguments for browse action. See browse function for the arguments.
   * \return The list of parameters.
   */
  QList<CControlPoint::TArgValue> browseArguments (QString const & objectID, EBrowseType type,
               QString const & filter, int startingIndex, int requestedCount, QString const & sortCriteria);

  /*! Prepares the arguments for search action. See search function for the arguments.
   * \return The list of parameters.
   */
  QList<CControlPoint::TArgValue> searchArguments (QString const & containerID,
        QString const & searchCriteria, QString const & filter,
        int startingIndex, int requestedCount, QString const & sortCriteria);

private :
  int m_browseTimeout = 20000; //!< Browse timeout to 20s.
  QSet<QString> m_validatedItemKeys;
  QSet<QString> m_parentIDs;
};

} // Namespace

#endif // CONTENT_DIRECTORY_HPP
