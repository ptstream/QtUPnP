#ifndef STATUS_HPP
#define STATUS_HPP

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"

START_DEFINE_UPNP_NAMESPACE

/*! The class CStatus holds information to simulate a set of 32 bits. */
class UPNP_API CStatus
{
public:
  /*! Default constructor. */
  inline CStatus () {}

  /*! Constructs with a st value. */
  inline CStatus (unsigned st);

  /*! Copy constructor. */
  inline CStatus (CStatus const & other);

  /*! Equal operator. */
  inline CStatus& operator = (CStatus const & other);

  /*! Equal operator. */
  inline CStatus& operator = (unsigned st);

  /*! Equality operator. */
  inline bool operator == (CStatus const & other);

  /*! Inequality operator. */
  inline bool operator != (CStatus const & other);

  /*! Adds a set of bits. */
  inline void addStatus (unsigned st);

  /*! Removes a set of bits. */
  inline void remStatus (unsigned st);

   /*! Has a set of bits. */
  inline bool hasStatus (unsigned st) const;

  /*! Return the set of bits. */
  inline unsigned status () const;

protected :
  unsigned m_status = 0; //!< The 32 bits
};

CStatus::CStatus (unsigned st) : m_status (st)
{
}

CStatus::CStatus (CStatus const & other) : m_status (other.m_status)
{
}

CStatus& CStatus::operator = (CStatus const & other)
{
  m_status = other.m_status;
  return *this;
}

CStatus& CStatus::operator = (unsigned st)
{
  m_status = st;
  return *this;
}

void CStatus::addStatus (unsigned st)
{
  m_status |= st;
}

void CStatus::remStatus (unsigned st)
{
  m_status &= ~st;
}

bool CStatus::hasStatus (unsigned st) const
{
  return (m_status & st) != 0;
}

unsigned CStatus::status () const
{
  return m_status;
}

bool CStatus::operator == (CStatus const & other)
{
  return m_status == other.m_status;
}

bool CStatus::operator != (CStatus const & other)
{
  return m_status != other.m_status;
}

inline bool operator == (CStatus const & s1, CStatus const & s2)
{
  return s1 == s2;
}

inline bool operator != (CStatus const & s1, CStatus const & s2)
{
  return s1 != s2;
}

} // Namespace

#endif // STATUS_HPP
