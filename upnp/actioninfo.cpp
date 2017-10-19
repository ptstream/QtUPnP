
#include "actioninfo.hpp"

START_DEFINE_UPNP_NAMESPACE

#define DStartEnvelopeBody \
"<s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" \
xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body>"

#define DActionserviceID "<u:%1 xmlns:u=\"%2\">"

#define DAction0 "</u:%1>"

#define DEndBodyEnvelope "</s:Body></s:Envelope>"

/*! \brief Internal structure of CActionInfo. */
struct SActionInfoData : public QSharedData
{
  SActionInfoData () {}
  SActionInfoData (SActionInfoData const & rhs);

  bool    m_success = false;
  QString m_deviceUUID;
  QString serviceID;
  QString m_actionName;
  QString m_message;
  QByteArray m_response;
};

SActionInfoData::SActionInfoData (SActionInfoData const & rhs) : QSharedData (rhs),
  m_success (rhs.m_success), m_deviceUUID (rhs.m_deviceUUID), serviceID (rhs.serviceID),
  m_actionName (rhs.m_actionName), m_message (rhs.m_message), m_response (rhs.m_response)
{
}

} // Namespace

USING_UPNP_NAMESPACE

CActionInfo::CActionInfo () : m_d (new SActionInfoData)
{
}

CActionInfo::CActionInfo (CActionInfo const & other) : m_d (other.m_d)
{
}

CActionInfo& CActionInfo::operator = (CActionInfo const & other)
{
  if (this != &other)
  {
    m_d.operator = (other.m_d);
  }

  return *this;
}

CActionInfo::~CActionInfo ()
{
}

void CActionInfo::startMessage (QString const & deviceUUID, QString const & serviceID, QString const & action)
{
  m_d->m_deviceUUID = deviceUUID;
  m_d->serviceID    = serviceID;
  m_d->m_actionName = action;
  m_d->m_message    = DStartEnvelopeBody + QString (DActionserviceID).arg (action).arg (serviceID);
}

void CActionInfo::endMessage ()
{
  m_d->m_message += QString (DAction0).arg (m_d->m_actionName) + QString (DEndBodyEnvelope);
}

void CActionInfo::addArgument (QString const & name, QString const & value)
{
  m_d->m_message += QString ("<%1>%2</%1>").arg (name, value);
}

bool CActionInfo::succeeded () const
{
  return m_d->m_success;
}

QString const & CActionInfo::deviceUUID () const
{
  return m_d->m_deviceUUID;
}

QString const & CActionInfo::serviceID () const
{
  return m_d->serviceID;
}

QString const & CActionInfo::actionName () const
{
  return m_d->m_actionName;
}

QString const & CActionInfo::message () const
{
  return m_d->m_message;
}

QByteArray const & CActionInfo::response () const
{
  return m_d->m_response;
}

void CActionInfo::setSucceeded (bool success)
{
  m_d->m_success = success;
}

void CActionInfo::setDeviceUUID (QString const & uuid)
{
  m_d->m_deviceUUID = uuid;
}

void CActionInfo::setserviceID (QString const & type)
{
  m_d->serviceID = type;
}

void CActionInfo::setActionName (QString const & name)
{
  m_d->m_actionName = name;
}

void CActionInfo::setMessage (QString const & message)
{
  m_d->m_message = message;
}

void CActionInfo::setResponse (QByteArray const & response)
{
  m_d->m_response = response;
}
