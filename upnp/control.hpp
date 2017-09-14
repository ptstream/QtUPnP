#ifndef CONTROL_HPP
#define CONTROL_HPP

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"
#include <QStringList>

START_DEFINE_UPNP_NAMESPACE

class CControlPoint;
class CActionInfo;

/*! \brief The base class for UPnP/AV specific objects (CConnectionManager, CAVTransport...).
 *
 * The CControl class is the base class for
 * \li CConnectionManager
 * \li CContentDirectory
 * \li
 * \li CRenderingContro
 */
class UPNP_API CControl
{
public:
  /*! Default constructor. */
  CControl () {}

  /*! Constructor. */
  CControl (CControlPoint* cp) : m_cp (cp) {}

  /*! Sets the application control point. */
  void setControlPoint (CControlPoint* cp) { m_cp = cp; }

  /*! Returns the control point. */
  CControlPoint* controlPoint () { return m_cp; }

  /*! Returns a string list corresponding at CVS format (values separated by comma). */
  QStringList fromCVS (QString entryString);

  /*! Returns a boolean value corresponding at a string.
   * True is returned if entryString doesn't start by '0' or 'f' or 'F';
  */
  bool toBool (QString const & entryString);

protected :
  CControlPoint* m_cp = nullptr; //<! The control point of the application.
};

} // Namespace

#endif // CONTROL_HPP
