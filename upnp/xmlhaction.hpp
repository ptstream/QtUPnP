#ifndef XMLH_ACTION_HPP
#define XMLH_ACTION_HPP

#include "xmlh.hpp"
#include "statevariable.hpp"

START_DEFINE_UPNP_NAMESPACE

/*! \brief Provides the implementation of the action response parser. */
class CXmlHAction : public CXmlH
{
public:
  /*! Default constructor. */
  CXmlHAction (QString const & actionName, QMap<QString, QString>& vars);

  /*! The parser calls this function when it has parsed a chunk of character data
   * See  QXmlContentHandler documentation.
   */
  virtual bool characters (QString const & name);

  /*! Returns the error code generates by the device in case of error. */
  int errorCode () const { return m_errorCode; }

  /*! Returns the error description generates by the device in case of error. */
  QString const & errorDesc () const { return m_errorDesc; }

private :
  QString m_actionName; //!< The name of the action.
  QMap<QString, QString>& m_vars; //!< The map of arguments.
  int m_errorCode = 0; //!< The error code sent by the device.
  QString m_errorDesc; //!< The error description sent by the device.
};

} // Namespace

#endif // XMLH_ACTION_HPP
