#ifndef XMLH_HPP
#define XMLH_HPP

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"
#include <QtXml/QXmlDefaultHandler>
#include <QStack>

START_DEFINE_UPNP_NAMESPACE

/*! \brief The base class of XML parsers. */
class CXmlH : public QXmlDefaultHandler
{
public:
  /*! Default constructor. */
  CXmlH ();

  /*! Constructor. */
  CXmlH (QStringList const & tags);

  /*! Destructor. */
  ~CXmlH ();

  /*! Parse the XML response. */
  virtual bool parse (QByteArray response);

  /*! Parse the XML response. */
  virtual bool parse (QString const & response);

  /*! The reader calls this function when it has parsed a start element tag.
   * See  QXmlContentHandler documentation.
   */
  virtual bool startElement (QString const &, QString const &, QString const & qName, QXmlAttributes const &);

  /*! The reader calls this function when it has parsed a end element tag.
   * See  QXmlContentHandler documentation.
   */
  virtual bool endElement (QString const &, QString const &, QString const &);

  /*! Replaces isolated char '&' by "&amp;".
   * Some renderer, use isolated char '&'. This stop the parser Qt xml parser.
   * For example, in the example below "&id=5786" stop the parser.
   * \code
   * <res protocolInfo="http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_TN">http://awox.vtuner.com/dynamOD.asp?ex45v=6eaada6e4bcdb71ec2fa37e1bcf14068&id=5786</res>
   * \endcode
   * \param data: Uncoded string.
   * \return Coded string.
  */
  static QString ampersandHandler (QString const & data);

  /*! Sets the tolerant mode.
   * \param mode: True the parser always return true after parsing.
   * False, the parser stop when it detects an error in the xml.
   */
  static void setTolerantMode (bool mode) { m_tolerantMode = mode; }

  /*! Returns the parsing tolerant mode.
   * \see setTolerantMode.
   */
  static bool tolerantMode () { return m_tolerantMode; }

protected :
  /*! Returns the current tag at a level. */
  QString tag (int level) const;

  /*! Returns the parent tag. */
  QString tagParent () const { return tag (1); }

  /*! Returns a string start by character '/'.
   * e.g. name="connection/xml" returns "/connection/xml".
   * if name begin by '/', this function make nothing.
   */
  static QString prependSlash (QString name);

  /*! Returns a string without name space specification.
   * e.g. name="s:connection" returns "connection"
   */
  static QString removeNameSpace (QString name);

protected :
  QStack<QString> m_stack; //!< Stack of tags.
  QStringList m_tags; //!< List of tag to handle
  static bool m_tolerantMode; //!< Defined the parsing mode.
};

} // Namespace

#endif // XMLH_HPP
