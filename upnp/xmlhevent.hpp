#ifndef XMLH_EVENT_HPP
#define XMLH_EVENT_HPP

#include "xmlh.hpp"
#include "httpserver.hpp"

START_DEFINE_UPNP_NAMESPACE

/*! \brief Provides the implementation of the event parser. */
class CXmlHEvent : public CXmlH
{
public:
  /*! Default constructor. */
  CXmlHEvent (TMEventVars& vars);

  /*! The reader calls this function when it has parsed a start element tag.
   * See  QXmlContentHandler documentation.
   */
  virtual bool startElement (QString const & namespaceURI, QString const & localName, QString const & qName, QXmlAttributes const & atts);

  /*! The parser calls this function when it has parsed a chunk of character data
   * See  QXmlContentHandler documentation.
   */
  virtual bool characters (QString const & name);

  void setCheckProperty (bool check) { m_checkProperty = check; }

private :
  bool m_success = false; //!< Parse success.
  bool m_checkProperty = true; //!< true=handle e:property tag, false=handle LastChange evented variable.
  TMEventVars& m_vars; //!< The last modified ariable.
};

} // Namespace

#endif // XMLH_EVENT_HPP
