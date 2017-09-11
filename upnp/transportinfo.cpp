#include "transportinfo.hpp"

START_DEFINE_UPNP_NAMESPACE

struct STransportInfoData : public QSharedData
{
  STransportInfoData () {}
  STransportInfoData (STransportInfoData const & other);

  QString m_currentTransportState;
  QString m_currentTransportStatus;
  QString m_currentSpeed = "1";
};

STransportInfoData::STransportInfoData (STransportInfoData const & other) : QSharedData (other),
          m_currentTransportState (other.m_currentTransportState),
          m_currentTransportStatus (other.m_currentTransportStatus),
          m_currentSpeed (other.m_currentSpeed)
{
}

}//Namespace

USING_UPNP_NAMESPACE

CTransportInfo::CTransportInfo () : m_d (new STransportInfoData)
{
}

CTransportInfo::CTransportInfo (CTransportInfo const & rhs) : m_d (rhs.m_d)
{
}

CTransportInfo& CTransportInfo::operator = (CTransportInfo const & rhs)
{
  if (this != &rhs)
  {
    m_d.operator = (rhs.m_d);
  }

  return *this;
}

CTransportInfo::~CTransportInfo ()
{
}

void CTransportInfo::setCurrentTransportState (QString const & state)
{
  m_d->m_currentTransportState = state;
}

void CTransportInfo::setCurrentTransportStatus (QString const & status)
{
  m_d->m_currentTransportStatus = status;
}

void CTransportInfo::setCurrentSpeed (QString const & speed)
{
  m_d->m_currentSpeed = speed;
}

QString const & CTransportInfo::currentTransportState () const
{
  return m_d->m_currentTransportState;
}

QString const & CTransportInfo::currentTransportStatus () const
{
  return m_d->m_currentTransportStatus;
}

QString const & CTransportInfo::currentSpeed () const
{
  return m_d->m_currentSpeed;
}


