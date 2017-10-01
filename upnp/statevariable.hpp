#ifndef STATE_VARIABLE_HPP
#define STATE_VARIABLE_HPP

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"
#include <QSharedDataPointer>
#include <QVariant>

START_DEFINE_UPNP_NAMESPACE

class SStateVariableData;

/*! Defines the constraint type. */
typedef QPair<QString, QString> TConstraint;

/*! \brief The CStateVariable class holds information about an UPnP state variable.
 *
 * The state variable values are updated by action and UPnP events.
 * The values may not be the values in the device.
 * The constraints are updated only by UPnP events. e.g. For the volume, the UPnP event
 * sends the constraint Channel=Master and the value.
 *
 * \remark Use implicit Sharing QT technology.
 */
class CStateVariable
{
public:
  /*! Variable type. */
  enum EType { Unknown, //!< Unknwon
               I1, //!< 8  bits integer
               Ui1, //!< 8 bits unsigned integer
               I2, //!< 16  bits integer
               Ui2, //!< 16 bits unsigned integer
               I4, //!< 32 bits integer
               Ui4, //!< 32 bits unsigned integer
               I8, //!< 64 bits integer
               Ui8, //!< 64 bits unsigned integer
               Real, //!< double
               String, //!< QString
               Boolean, //!< bool
             };

  /*! Default constructor. */
  CStateVariable ();

  /*! Copy constructor. */
  CStateVariable (CStateVariable  const & other);

  /*! Equal operator. */
  CStateVariable& operator = (CStateVariable  const & other);

  /*! Destructor. */
  ~CStateVariable ();

  /*! Sets the variable type. */
  void setType (EType type);

  /*! Sets the variable type from QString. */
  void setType (QString const & stype);

  /*! Sets the variable is evented. */
  void setEvented (bool evented);

  /*! Sets the allowed values. */
  void setAllowedValues (QStringList const & allowed);

  /*! Adds an allowed value. */
  void addAllowedValue (QString const & allowed);

  /*! Sets the value minimum. */
  void setMinimum (double minimum);

  /*! Sets the value maximum. */
  void setMaximum (double maximum);

  /*! Sets the variable step. */
  void setStep (double step);

  /*! Sets the value. */
  void setValue (QString const & value);

  /*! Sets the variable constraints. */
  void setConstraints (QList<TConstraint> const & csts);

  /*! Returns the variable type. */
  EType type () const;

  /*! Returns the variable is evented. */
  bool isEvented () const;

  /*! Returns the allowed values. */
  QStringList allowedValues () const;

  /*! Returns the value minimum. */
  double minimum () const;

  /*! Returns the value maximum. */
  double maximum () const;

  /*! Returns the variable step. */
  double step () const;

  /*! Returns the value. */
  QVariant const & value () const;

  /*! Returns the variable constraints as a const reference. */
  QList<TConstraint> const & constraints () const;

  /*! Returns the variable constraints as a reference. */
  QList<TConstraint> & constraints ();

  /*! Returns true if the variable has not the type Unknown. */
  bool isValid () const;

  /*! Return the value as a QString. */
  QString toString () const;

  /*! Returns the type from QString. */
  static EType typeFromString (QString const & stype);

private:
  QSharedDataPointer<SStateVariableData> m_d; //!< Shared data pointer.
};

} // Namespace

#endif // STATE_VARIABLE_HPP
