#include "action.hpp"

START_DEFINE_UPNP_NAMESPACE

/*! \brief Internal structure of CAction. */
struct SActionData : public QSharedData
{
  SActionData () {}
  SActionData (SActionData const & other);

  TMArguments m_arguments; //!< Map of arguments.
};

SActionData::SActionData (SActionData const & other) : QSharedData (other), m_arguments (other.m_arguments)
{
}

} // Namespace

USING_UPNP_NAMESPACE

CAction::CAction () : m_d (new SActionData)
{
}

CAction::CAction (CAction const & rhs) : m_d (rhs.m_d)
{
}

CAction &CAction::operator = (CAction const & other)
{
  if (this != &other)
  {
    m_d.operator = (other.m_d);
  }

  return *this;
}

CAction::~CAction ()
{
}

void CAction::setArguments (TMArguments const & args)
{
  m_d->m_arguments = args;
}

TMArguments const & CAction::arguments () const
{
  return m_d->m_arguments;
}

TMArguments& CAction::arguments()
{
  return m_d->m_arguments;
}
