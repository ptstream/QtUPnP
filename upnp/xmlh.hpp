#ifndef XMLH_HPP
#define XMLH_HPP

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"
#include <QtXml/QXmlDefaultHandler>
#include <QStack>

START_DEFINE_UPNP_NAMESPACE

/*! This class is the base class of XML parsers. */
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
  virtual bool parse (QString response);

  /*! The reader calls this function when it has parsed a start element tag.
   * See  QXmlContentHandler documentation.
   */
  virtual bool startElement (QString const &, QString const &, QString const & qName, QXmlAttributes const &);

  /*! The reader calls this function when it has parsed a end element tag.
   * See  QXmlContentHandler documentation.
   */
  virtual bool endElement (QString const &, QString const &, QString const &);

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
};

} // Namespace

#endif // XMLH_HPP
