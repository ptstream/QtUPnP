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
  enum EQueryType { Unknown,
                    Playlist,
                    Plugin,
                  };

  enum EContentLength { Unreached = -2,
                        Chunked   = -1,
                      };

  /*! Constructor. */
  CHTTPParser () {}

  /*! Copy constructor. */
  CHTTPParser (CHTTPParser const & other);

  /*! Constructs a parser with the message to parse. */
  CHTTPParser (QByteArray const & message) : m_message (message) {}

  CHTTPParser& operator = (CHTTPParser const & other);

  void setMessage (QByteArray const & message) { m_message = message; }

  int headerLengthReached () const;
  int headerContentLength () const;

  /*! Returns the header length including "\r\n\r\n" at the end. */
  int headerLength () const { return m_headerLength; }

  /*! Returns the content length. */
  int contentLength () const { return m_contentLength; }

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
  bool parseMessage();

  /*! Returns if the header contains TRANSFER-ENCODING: chunked or CONTENT-LENGTH. */
  bool transferChunked () const;

  EQueryType queryType () const { return m_queryType; }

  QByteArray const & message () const { return m_message; }
  QByteArray& message () { return m_message; }

  /*! Returns the query type from the query. */
  static EQueryType queryType (QByteArray const & query);

private :
  QByteArray m_message; //!< The response.
  QByteArray m_verb; //!< The verb
  QMap<QByteArray, QByteArray> m_headerElems; //!< The headers map of rows.
  EQueryType m_queryType = Unknown; //!< Type of query.
  int m_headerLength = 0, m_contentLength = 0;

  static char const * m_playlistSuffixes[]; //!< Play list suffixes (m3u, m3u8...)
};

} // Namespace

#endif // HTTP_PARSER_HPP
