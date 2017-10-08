#include "statevariable.hpp"

START_DEFINE_UPNP_NAMESPACE

/*! \brief Internal structure of CStateVariable. */
struct SStateVariableData : public QSharedData
{
  SStateVariableData () { m_values.push_back (TValue ()); }
  SStateVariableData (SStateVariableData const & other);

  int m_type = 0;
  bool m_evented = false;
  QStringList m_allowedValues;
  double m_minimum = 0, m_maximum = 0, m_step = 0;
  QList<TValue> m_values;
 };

SStateVariableData::SStateVariableData (SStateVariableData const & other) : QSharedData (other),
  m_type (other.m_type), m_evented (other.m_evented), m_allowedValues (other.m_allowedValues),
  m_minimum (other.m_minimum), m_maximum (other.m_maximum), m_step (other.m_step),
  m_values (other.m_values)
{
  if (m_values.isEmpty ())
  {
    m_values.push_back (TValue ());
  }
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

bool CStateVariable::sameConstraints (QList<TConstraint> const & c1s, QList<TConstraint> const & c2s)
{
  bool same = c1s.size () == c2s.size ();
  if (same)
  {
    int count = 0;
    for (TConstraint const & c1 : c1s)
    {
      for (TConstraint const & c2 : c2s)
      {
        if (c1.first.compare (c2.first, Qt::CaseInsensitive) == 0 &&
            c1.second.compare (c2.second, Qt::CaseInsensitive) == 0)
        {
          ++count;
        }
      }
    }

    same = count == c1s.size ();
  }

  return same;
}

QVariant CStateVariable::convert (QString const & value)
{
  bool     ok;
  QVariant variant;
  switch (m_d->m_type)
  {
    case I1 :
    {
      short i1 = value.toShort (&ok);
      if (ok)
      {
        variant = static_cast<qint8>(i1);
      }
      break;
    }

    case Ui1 :
    {
      unsigned short ui1 = value.toUShort (&ok);
      if (ok)
      {
        variant = static_cast<quint8>(ui1);
      }
      break;
    }

    case I2 :
    {
      short i2 = value.toShort (&ok);
      if (ok)
      {
        variant = i2;
      }
      break;
    }

    case Ui2 :
    {
      unsigned short ui2 = value.toUShort (&ok);
      if (ok)
      {
        variant = ui2;
      }
      break;
    }

    case I4 :
    {
      int i4 = value.toInt (&ok);
      if (ok)
      {
        variant = i4;
      }
      break;
    }

    case Ui4 :
    {
      unsigned ui4 = value.toUInt (&ok);
      if (ok)
      {
        variant = ui4;
      }
      break;
    }

    case I8 :
    {
      long long i8 = value.toLongLong (&ok);
      if (ok)
      {
        variant = i8;
      }
      break;
    }

    case Ui8 :
    {
      unsigned long long ui8 = value.toULongLong (&ok);
      if (ok)
      {
        variant = ui8;
      }
      break;
    }

    case Real :
    {
      double real = value.toDouble (&ok);
      if (ok)
      {
        variant = real;
      }
      break;
    }

    case String :
      variant = value;
      break;

    case Boolean :
    {
      bool boolean = false;
      if (!value.isEmpty ())
      {
        char c  = value[0].toLatin1 ();
        boolean = c == '1' || c == 't' || c == 'T' || c == 't' || c == 'Y';
      }

      variant = boolean;
      break;
    }
  }

  return variant;
}

void CStateVariable::setValue (QString const & value, QList<TConstraint> const & constraints)
{
  bool update = false;
  for (QList<TValue>::iterator it = m_d->m_values.begin (), end = m_d->m_values.end (); it != end; ++it)
  {
    TValue&             val                = (*it);
    QList<TConstraint>& variableContraints = val.second;
    if (sameConstraints (variableContraints, constraints))
    {
      update    = true;
      val.first = convert (value);
      break;
    }
  }

  if (!update)
  {
    QVariant variant = convert (value);
    m_d->m_values.push_back (TValue (variant, constraints));
  }
}

void CStateVariable::setValue (QString const & value)
{
  m_d->m_values.first ().first = convert (value);
}

void CStateVariable::setConstraints (QList<TConstraint> const & csts)
{
  m_d->m_values.first ().second = csts;
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

QVariant CStateVariable::value (QList<TConstraint> const & constraints) const
{
  QVariant variant;
  for (QList<TValue>::const_iterator it = m_d->m_values.cbegin (), end = m_d->m_values.cend (); it != end; ++it)
  {
    TValue const &             val                = (*it);
    QList<TConstraint> const & variableContraints = val.second;
    if (sameConstraints (variableContraints, constraints))
    {
      variant = val.first;
      break;
    }
  }

  return variant;
}

QVariant const & CStateVariable::value () const
{
  return m_d->m_values.first ().first; // At least one value exists.
}

QList<TConstraint>& CStateVariable::constraints ()
{
  return m_d->m_values.first ().second; // At least one value exists.
}

QList<TConstraint> const & CStateVariable::constraints () const
{
  return m_d->m_values.first ().second; // At least one value exists.
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
  else if (stype == "i1")
  {
    t = CStateVariable::I1;
  }
  else if (stype == "ui1")
  {
    t = CStateVariable::Ui1;
  }
  else if (stype == "i2")
  {
    t = CStateVariable::I2;
  }
  else if (stype == "ui2")
  {
    t = CStateVariable::Ui4;
  }
  else if (stype == "i4")
  {
    t = CStateVariable::I4;
  }
  else if (stype == "ui4")
  {
    t = CStateVariable::Ui4;
  }
  else if (stype == "i8")
  {
    t = CStateVariable::I8;
  }
  else if (stype == "ui8")
  {
    t = CStateVariable::Ui8;
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
  return m_d->m_values.first ().first.toString ();
}
