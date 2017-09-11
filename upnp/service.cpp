#include "service.hpp"
#include "xmlhservice.hpp"

START_DEFINE_UPNP_NAMESPACE

struct SServiceData : public QSharedData
{
  SServiceData () {}
  SServiceData (SServiceData const & other);

  QString m_serviceType;
  QString m_controlURL;
  QString m_eventSubURL;
  QString m_scpdURL;
  QString m_subscribeSID;
  TMStateVariables m_stateVariables;
  TMActions m_actions;
  QVector<unsigned> m_instanceIDs = { 0 };
  unsigned short m_minorVersion = 0;
  unsigned short m_majorVersion = 1;
  bool m_evented = false;
};

SServiceData::SServiceData (SServiceData const & other) :  QSharedData (other),
   m_serviceType (other.m_serviceType), m_controlURL (other.m_controlURL),
   m_eventSubURL (other.m_eventSubURL), m_scpdURL (other.m_scpdURL), m_subscribeSID (other.m_subscribeSID),
   m_stateVariables (other.m_stateVariables), m_actions (other.m_actions),
   m_instanceIDs (other.m_instanceIDs),
   m_minorVersion (other.m_minorVersion), m_majorVersion (other.m_majorVersion),
   m_evented (other.m_evented)
{
}

} // Namespace

USING_UPNP_NAMESPACE

CService::CService () : m_d (new SServiceData)
{
}

CService::CService (CService const & other) : m_d (other.m_d)
{
}

CService &CService::operator = (CService const & other)
{
  if (this != &other)
  {
    m_d.operator = (other.m_d);
  }

  return *this;
}

CService::~CService ()
{
}

void CService::setServiceType (QString const & type)
{
  m_d->m_serviceType = type;
}

void CService::setControlURL (QString const & url)
{
  m_d->m_controlURL = url;
}

void CService::setEventSubURL (QString const & url)
{
  m_d->m_eventSubURL = url;
}

void CService::setScpdURL (QString const & url)
{
  m_d->m_scpdURL = url;
}

void CService::setSubscribeSID (QString const & id)
{
  m_d->m_subscribeSID = id;
}

void CService::setInstanceIDs (QVector<unsigned> const & ids)
{
  m_d->m_instanceIDs = ids;
}

void CService::setStateVariables (TMStateVariables const & stateVariables)
{
  m_d->m_stateVariables = stateVariables;
}

void CService::setActions (TMActions const & actions)
{
  m_d->m_actions = actions;
}

void CService::setMajorVersion (unsigned version)
{
  m_d->m_majorVersion = static_cast<unsigned short>(version);
}

void CService::setMinorVersion (unsigned version)
{
  m_d->m_minorVersion = static_cast<unsigned short>(version);
}

void CService::setEvented (bool evented)
{
  m_d->m_evented = evented;
}

QString const & CService::serviceType () const
{
  return m_d->m_serviceType;
}

QString const & CService::controlURL () const
{
  return m_d->m_controlURL;
}

QString const & CService::eventSubURL () const
{
  return m_d->m_eventSubURL;
}

QString const & CService::scpdURL () const
{
  return m_d->m_scpdURL;
}

QString const & CService::subscribeSID () const
{
  return m_d->m_subscribeSID;
}

QVector<unsigned> const & CService::instanceIDs () const
{
  return m_d->m_instanceIDs;
}

TMStateVariables const & CService::stateVariables () const
{
  return m_d->m_stateVariables;
}

TMStateVariables& CService::stateVariables ()
{
  return m_d->m_stateVariables;
}

TMActions const & CService::actions () const
{
  return m_d->m_actions;
}

TMActions & CService::actions ()
{
  return m_d->m_actions;
}

unsigned CService::minorVersion () const
{
  return m_d->m_minorVersion;
}

unsigned CService::highestSupportedVersion () const
{
  unsigned version = 0;
  int      index   = m_d->m_serviceType.lastIndexOf (':');
  if (index != -1)
  {
    QString s = m_d->m_serviceType.right (m_d->m_serviceType.length () - index - 1);
    version   = s.toUInt ();
  }

  return version;
}

unsigned CService::majorVersion () const
{
  return m_d->m_majorVersion;
}

bool CService::isEvented () const
{
  return m_d->m_evented;
}

void CService::clearSID ()
{
  m_d->m_subscribeSID.clear ();
}

bool CService::parseXml (QByteArray const & data)
{
  CXmlHService h (*this);
  bool         success = h.parse (data);
  return success;
}

CStateVariable CService::stateVariable (QString const & name) const
{
  return m_d->m_stateVariables.value (name);
}
