#ifndef XMLH_DIDL_LITE_HPP
#define XMLH_DIDL_LITE_HPP

#include "xmlh.hpp"
#include "didlitem.hpp"

START_DEFINE_UPNP_NAMESPACE

/*! This class provides the implementation of the DIDL-Lite server response parser.
 * At the end of the parsing, this class contains the item list and their elements.
 */
class CXmlHDidlLite : public CXmlH
{
public:
  /*! Default constructor. */
  CXmlHDidlLite ();

  /*! The reader calls this function when it has parsed a start element tag.
   * See  QXmlContentHandler documentation.
   */
  virtual bool startElement (QString const & namespaceURI, QString const & localName, QString const & qName, QXmlAttributes const & atts);

  /*! The parser calls this function when it has parsed a chunk of character data
   * See  QXmlContentHandler documentation.
   */
  virtual bool characters (QString const & name);

  /*! Parse the XML response. */
  virtual bool parse (QByteArray response);

  /*! Parse the XML response. */
  virtual bool parse (QString response);

  /*! Returns the lists of item components as a constant reference. */
  QList<CDidlItem> const & items () const { return m_items; }

  /*! Returns the first item of the item list. */
  CDidlItem firstItem (QString data);

  /*! Returns the first item of the item list. */
  CDidlItem firstItem (QByteArray data);

  void sortElems ();

private :
  QList<CDidlItem> m_items; //!< item list. See CDidlIem.
};

} // Namespace

#endif // XMLH_DIDL_LITE_HPP
