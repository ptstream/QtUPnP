#include "argument.hpp"

START_DEFINE_UPNP_NAMESPACE

struct SArgumentData : public QSharedData
{
  SArgumentData () {}
  SArgumentData (SArgumentData const & rhs);

  int m_dir; //!< Argument direction.
  QString m_relatedStateVariable; //!< The related variable name.
};

SArgumentData::SArgumentData (SArgumentData const & rhs) : QSharedData (rhs),
  m_dir (rhs.m_dir), m_relatedStateVariable (rhs.m_relatedStateVariable)
{
}

} // Namespace

USING_UPNP_NAMESPACE

CArgument::CArgument () : m_d (new SArgumentData)
{
}

CArgument::CArgument (const CArgument & other) : m_d (other.m_d)
{
}

CArgument& CArgument::operator = (CArgument const & other)
{
  if (this != &other)
  {
    m_d.operator = (other.m_d);
  }

  return *this;
}

CArgument::~CArgument ()
{
}

CArgument::EDir CArgument::dir () const
{
  return static_cast<EDir>(m_d->m_dir);
}

QString const & CArgument::relatedStateVariable () const
{
  return m_d->m_relatedStateVariable;
}

void CArgument::setDir (EDir dir)
{
  m_d->m_dir = dir;
}

void CArgument::setRelatedStateVariable (QString const & var)
{
  m_d->m_relatedStateVariable = var;
}
