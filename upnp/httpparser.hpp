#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"
#include <QMap>
#include <QByteArray>

START_DEFINE_UPNP_NAMESPACE

/*! \brief A very simple HTTP parser used by eventing.
 *
 * All member functions must are called after having called parseMessage function.
 */
class UPNP_API CHTTPParser
{
public :
  /*! Constructor. */
  CHTTPParser () {}

  /*! Constructs a parser with the message to parse. */
  CHTTPParser (QByteArray const & message) : m_message (message) {}

  /*! Returns the header length including "\r\n\r\n" at the end. */
  int headerLength () const;

  /*! Returns the content length. */
  int contentLength () const;

  /*! Returns the value of an header row.
   * \param name: The row name.
   * \return The value.
   */
  QByteArray value (char const * name) const { return m_headerElems.value (name); }

  /*! Returns the value of an SID row. */
  QByteArray sid () const { return value ("SID"); }

  /*! Returns the verb. */
  QByteArray verb () const { return m_verb; }

  /*! Parse the response. */
  void parseMessage ();

  /*! Returns true if the verb value defines a playlist. */
  bool isPlaylistPath () const { return m_playlistPath; }

  /*! Returns if the header contains TRANSFER-ENCODING: chunked or CONTENT-LENGTH. */
  bool transferChunked () const;

  /*! Returns true if path defines a playlist. */
  static bool isPlaylistPath (QString const & path);

private :
  QByteArray m_message; //!< The response.
  QByteArray m_verb; //!< The verb
  QMap<QByteArray, QByteArray> m_headerElems; //!< The headers map of rows.
  bool m_playlistPath = false; //!< The verb value is a point a playlist.

  static char const * m_playlistSuffixes[]; //!< Play list suffixes (m3u, m3u8...)
};

} // Namespace

#endif // HTTP_PARSER_HPP
