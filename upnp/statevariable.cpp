#include "statevariable.hpp"

START_DEFINE_UPNP_NAMESPACE

/*! \brief Internal structure of CStateVariable. */
struct SStateVariableData : public QSharedData
{
  SStateVariableData () {}
  SStateVariableData (SStateVariableData const & other);

  int m_type = 0;
  bool m_evented = false;
  QStringList m_allowedValues;
  double m_minimum = 0, m_maximum = 0, m_step = 0;
  QVariant m_value;
  QList<TConstraint> m_constraints;
};

SStateVariableData::SStateVariableData (SStateVariableData const & other) : QSharedData (other),
  m_type (other.m_type), m_evented (other.m_evented), m_allowedValues (other.m_allowedValues),
  m_minimum (other.m_minimum), m_maximum (other.m_maximum), m_step (other.m_step),
  m_value (other.m_value), m_constraints (other.m_constraints)
{
}

} // Namespace

USING_UPNP_NAMESPACE

CStateVariable::CStateVariable () : m_d (new SStateVariableData)
{
}

CStateVariable::CStateVariable (CStateVariable  const &  other) : m_d (other.m_d)
{
}

CStateVariable &CStateVariable::operator = (CStateVariable  const &  other)
{
  if (this != &other)
  {
    m_d.operator = (other.m_d);
  }

  return *this;
}

CStateVariable::~CStateVariable ()
{
}

void CStateVariable::setType (EType type)
{
  m_d->m_type = type;
}

void CStateVariable::setType (QString const & stype)
{
  m_d->m_type = typeFromString (stype);
}

void CStateVariable::setEvented (bool evented)
{
  m_d->m_evented = evented;
}

void CStateVariable::setAllowedValues (QStringList const & allowed)
{
  m_d->m_allowedValues = allowed;
}

void CStateVariable::addAllowedValue (QString const & allowed)
{
  m_d->m_allowedValues << allowed;
}

void CStateVariable::setMinimum (double minimum)
{
  m_d->m_minimum = minimum;
}

void CStateVariable::setMaximum (double maximum)
{
  m_d->m_maximum = maximum;
}

void CStateVariable::setStep (double step)
{
  m_d->m_step = step;
}

void CStateVariable::setValue (QVariant const & v)
{
  m_d->m_value = v;
}

void CStateVariable::setConstraints (QList<TConstraint> const & csts)
{
  m_d->m_constraints = csts;
}

CStateVariable::EType CStateVariable::type () const
{
  return static_cast<CStateVariable::EType>(m_d->m_type);
}

bool CStateVariable::isEvented () const
{
  return m_d->m_evented;
}

QStringList CStateVariable::allowedValues () const
{
  return m_d->m_allowedValues;
}

double CStateVariable::minimum () const
{
  return m_d->m_minimum;
}

double CStateVariable::maximum () const
{
  return m_d->m_maximum;
}

double CStateVariable::step () const
{
  return m_d->m_step;
}

QVariant const & CStateVariable::value () const
{
  return m_d->m_value;
}

QList<TConstraint>& CStateVariable::constraints ()
{
  return m_d->m_constraints;
}

QList<TConstraint> const & CStateVariable::constraints () const
{
  return m_d->m_constraints;
}

bool CStateVariable::isValid () const
{
  return m_d->m_type != Unknown;
}

CStateVariable::EType CStateVariable::typeFromString (QString const & stype)
{
  CStateVariable::EType t = CStateVariable::Unknown;
  if (stype == "string")
  {
    t = CStateVariable::String;
  }
  else if (stype == "i4")
  {
    t = CStateVariable::I4;
  }
  else if (stype == "ui4")
  {
    t = CStateVariable::Ui4;
  }
  else if (stype == "real")
  {
    t = CStateVariable::Real;
  }
  else if (stype == "boolean")
  {
    t = CStateVariable::Boolean;
  }

  return t;
}

QString CStateVariable::toString () const
{
  return m_d->m_value.toString ();
}
