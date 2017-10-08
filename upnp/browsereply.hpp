#ifndef BROWSE_REPLY_HPP
#define BROWSE_REPLY_HPP

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"
#include "didlitem.hpp"
#include <QSharedDataPointer>

START_DEFINE_UPNP_NAMESPACE

/*! Forward declaration of CBrowseReply data. */
struct SBrowseReplyData;

/*! \brief The CBrowseReply class holds information about the response of Browse and Search action.
 *
 * The class CAVTransport is a wrapper that manages the urn:schemas-upnp-org:service:AVTransport:1
 * service actions.
 * From standard C++ aguments, the member functions invoke the corresponding action and return
 * the results in QtUPnP classes.
 * See http://upnp.org/specs/av/UPnP-av-ContentDirectory-Service.pdf.
 *
 * \remark Use implicit Sharing QT technology.
 */
class UPNP_API CBrowseReply
{
public :
  /*! Sort direction. */
  enum ESortDir { ascending, descending };

  /*! Default constructor. */
  CBrowseReply ();

  /*! Copy constructor. */
  CBrowseReply (CBrowseReply const & other);

  /*! Equal operator. */
  CBrowseReply& operator = (CBrowseReply const & other);

  /*! Destructor. */
  ~CBrowseReply ();

  /*! Sets the returnd number of CIdlItem. */
  void setNumberReturned (unsigned numberReturned);

  /*! Sets the total matches number of CIdlItem. */
  void setTotalMatches (unsigned totalMatches);

  /*! Sets the update identifier. */
  void setUpdateID (unsigned updateID);

  /*! Sets the list of CDidlItems. */
  void setItems (QList<CDidlItem> const & items);

  /*! Returns the returnd number of CIdlItem. */
  unsigned numberReturned () const;

  /*! Returns the total matches number of CIdlItem. */
  unsigned totalMatches () const;

  /*! Returns the update identifier. */
  unsigned updateID () const;

  /*! Returns the list of CDidlItems as a const reference. */
  QList<CDidlItem> const & items () const;

  /*! Returns the list of CDidlItems as a reference. */
  QList<CDidlItem>& items ();

  /*! Add the other content at the content of this. */
  CBrowseReply& operator += (CBrowseReply const & other);

  /*! Sorts the item list by criteria.
   * \param criteria: The criteria is the CDidlElem name or property.
   * To sort by CDidlElem name, the criteria must be the complet name (e.g. to sort by title, the criteria is "dc:title").
   * To sort by property, the criteria must have the form "name@property".
   * \param dir: ascending or descending see ESortDir.
   * E.g. To sort by duration, the criteria is "res@duration".
   */
  void sort (QString const & criteria, ESortDir dir = ascending);

  /*! returns the sort capabilities. It is the list of criterias for sort function.
   * \param reply: Detect the sort capabilities from CBrowseReply content.
   * \param sameContent: True assumes all CDidlElem contents are identical.
   * In this case, the function use only the first CDidlItem.
   */
  static QStringList sortCapabilities (CBrowseReply const & reply, bool sameContent = true);

   /*! returns the sort capabilities. It is the list of criterias for sort function.
   * \param sameContent: True assumes all CDidlElem contents are identical.
   * In this case, the function use all CDidlItem elems.
   */
  QStringList sortCapabilities (bool sameContent = true) const;

  /*! Sorts the item list by criteria.
   * \param reply: CBrowseReply to sort.
   * \param criteria: The criteria is the CDidlElem name or property.
   * To sort by CDidlElem name, the criteria must be the complet name (e.g. to sort by title, the criteria is "dc:title").
   * To sort by property, the criteria must have the form "name@property".
   * E.g. To sort by duration, the criteria is res\@duration.
   * \param dir: ascending or descending see ESortDir.
   */
  static void sort (CBrowseReply& reply, QString const & criteria, ESortDir dir = ascending);

  /*! Returns the CDidlItem list that match exactly or approximately text.
   * This function search the title that best corresponds to the text entered and returns
   * the list sorted by better match.
   * \param text: Text to search.
   * \param commonPrefixLength; Winkler common prefix see jaroWinklerDistance in helper.hpp.
   * \param returned: Max number of CDidlItem return. If returned <= 0 or > items size, returnd whill be items size.
   * \return The list of sorted CDidlItem.
   */
  QList<CDidlItem> search (QString text, int returned = -1, int commonPrefixLength = -1) const;

  /*! Returns the CDidlItem list that match exactly or approximately text.
   * This function search the title that best corresponds to the text entered and returns
   * the list sorted by better match.
   * \param items: Search in items by title.
   * \param text: Text to search.
   * \param commonPrefixLength; Winkler common prefix see jaroWinklerDistance in helper.hpp.
   * \param returned: Max number of CDidlItem return. If returned <= 0 or > items size, returnd whill be items size.
   * \return The list of sorted CDidlItem.
   */
  static QList<CDidlItem> search (QList<CDidlItem> const & items, QString text, int returned = -1,
                                  int commonPrefixLength = -1);

  /*! Converts the content of this to a QStringList. */
  QStringList dump ();

  /*! Write the content to QDebug. */
  void debug ();

private:
  QSharedDataPointer<SBrowseReplyData> m_d; //!< Shared data pointer.
};

} //Namespace

#endif // BROWSE_REPLY_HPP
