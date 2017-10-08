#ifndef ARGUMENT_HPP
#define ARGUMENT_HPP

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"
#include <QSharedDataPointer>

START_DEFINE_UPNP_NAMESPACE

/*! Forward CArgument data. */
struct SArgumentData;

/*! \brief The CArgument class holds information about an action argument.
 *
 * \remark Use implicit Sharing QT technology.
 */
class UPNP_API CArgument
{
public:
 /*! Direction of the variable. */
  enum EDir { Unknown, //!< Unknown direction.
              In, //!< The value is sent.
              Out //!< The value is returned.
            };

  /*! Defaut constructor. */
  CArgument ();

  /*! Copy constructor. */
  CArgument (CArgument const & other);

  /*! Equal operator. */
  CArgument& operator = (CArgument const & other);
  ~CArgument ();

  /*! Returns the direction of the argument. */
  EDir dir () const;

  /*! Returns the related variable name. */
  QString const & relatedStateVariable () const;

  /*! Sets the direction of the argument. */
  void setDir (EDir dir);

  /*! Sets the name of the related variable. */
  void setRelatedStateVariable (QString const & var);

private:
  QSharedDataPointer<SArgumentData> m_d; //!< Shared data pointer.
};

} // Namespace

#endif // ARGUMENT_HPP
