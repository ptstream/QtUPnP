#include "connectioninfo.hpp"

START_DEFINE_UPNP_NAMESPACE

/*! \brief Internal strucure for CConnectionInfo. */
struct SConnectionInfoData : public QSharedData
{
  SConnectionInfoData () {}
  SConnectionInfoData (SConnectionInfoData const & other);

  int m_rcsID = -1;
  int m_avTransportID = -1;
  QString m_protocolInfo;
  QString m_peerConnectionManager;
  int m_peerConnectionID = 0;
  QString m_direction = "Unknown";
  QString m_status = "Unknown";
};

SConnectionInfoData::SConnectionInfoData (SConnectionInfoData const & other) : QSharedData (other),
          m_rcsID (other.m_rcsID), m_avTransportID (other.m_avTransportID), m_protocolInfo (other.m_protocolInfo),
          m_peerConnectionManager (other.m_peerConnectionManager), m_peerConnectionID (other.m_peerConnectionID),
          m_direction (other.m_direction), m_status (other.m_status)
{
}

}//Namespace

USING_UPNP_NAMESPACE

CConnectionInfo::CConnectionInfo () : m_d (new SConnectionInfoData)
{
}

CConnectionInfo::CConnectionInfo (CConnectionInfo const & rhs) : m_d (rhs.m_d)
{
}

CConnectionInfo& CConnectionInfo::operator = (CConnectionInfo const & rhs)
{
  if (this != &rhs)
  {
    m_d.operator = (rhs.m_d);
  }

  return *this;
}

CConnectionInfo::~CConnectionInfo ()
{
}

void CConnectionInfo::setRcsID (int id)
{
  m_d->m_rcsID = id;
}

void CConnectionInfo::setAVTransportID (int id)
{
  m_d->m_avTransportID = id;
}

void CConnectionInfo::setProtocolInfo (QString const & protocol)
{
  m_d->m_protocolInfo = protocol;
}

void CConnectionInfo::setPeerConnectionManager (QString const & pcm)
{
  m_d->m_peerConnectionManager = pcm;
}

void CConnectionInfo::setPeerConnectionID (int id)
{
  m_d->m_peerConnectionID = id;
}

void CConnectionInfo::setDirection (QString const & dir)
{
  m_d->m_direction = dir;
}

void CConnectionInfo::setStatus (QString const & st)
{
  m_d->m_status = st;
}

int CConnectionInfo::rcsID () const
{
  return m_d->m_rcsID;
}

int CConnectionInfo::avTransportID () const
{
  return m_d->m_avTransportID;
}

QString const & CConnectionInfo::protocolInfo () const
{
  return m_d->m_protocolInfo;
}

QString const & CConnectionInfo::peerConnectionManager () const
{
  return m_d->m_peerConnectionManager;
}

int CConnectionInfo::peerConnectionID () const
{
  return m_d->m_peerConnectionID;
}

QString const & CConnectionInfo::direction () const
{
  return m_d->m_direction;
}

QString const & CConnectionInfo::status () const
{
  return m_d->m_status;
}
