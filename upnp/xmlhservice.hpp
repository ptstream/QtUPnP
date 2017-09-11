#ifndef XMLH_SERVICE_HPP
#define XMLH_SERVICE_HPP

#include "xmlh.hpp"
#include "action.hpp"
#include "statevariable.hpp"

START_DEFINE_UPNP_NAMESPACE

class CService;

/*! This class provides the implementation of the service response parser. */
class CXmlHService : public CXmlH
{
public:
  /*! Default constructor. */
  CXmlHService (CService& service);

  /*! The reader calls this function when it has parsed a start element tag.
   * See  QXmlContentHandler documentation.
   */
  virtual bool startElement (QString const & namespaceURI, QString const & localName, QString const & qName, QXmlAttributes const & atts);

  /*! The parser calls this function when it has parsed a chunk of character data
   * See  QXmlContentHandler documentation.
   */
  virtual bool characters (QString const & name);

  /*! The reader calls this function when it has parsed a end element tag.
   * See  QXmlContentHandler documentation.
   */
  bool endElement (QString const & namespaceURI, QString const & localName, QString const & qName);

private :
  QPair<QString, CStateVariable> m_var; //!< The couple name-state variable.
  QPair<QString, CAction> m_action; //!< The couple name-action.
  QPair<QString, CArgument> m_arg; //!< The couple name-argument.
  CService& m_service; //!< The service to update.
};

} // Namespace

#endif // XMLH_SERVICE_HPP
