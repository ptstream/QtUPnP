#include "transportsettings.hpp"

START_DEFINE_UPNP_NAMESPACE

struct STransportSettingsData : public QSharedData
{
  STransportSettingsData () {}
  STransportSettingsData (STransportSettingsData const & other);

  QString m_playMode;
  QString m_recQualityMode;
};

STransportSettingsData::STransportSettingsData (STransportSettingsData const & other) : QSharedData (other),
          m_playMode (other.m_playMode),
          m_recQualityMode (other.m_recQualityMode)
{
}

}//Namespace

USING_UPNP_NAMESPACE

CTransportSettings::CTransportSettings () : m_d (new STransportSettingsData)
{
}

CTransportSettings::CTransportSettings (CTransportSettings const & rhs) : m_d (rhs.m_d)
{
}

CTransportSettings& CTransportSettings::operator = (CTransportSettings const & rhs)
{
  if (this != &rhs)
  {
    m_d.operator = (rhs.m_d);
  }

  return *this;
}

CTransportSettings::~CTransportSettings ()
{
}

void CTransportSettings::setPlayMode (QString const & mode)
{
  m_d->m_playMode = mode;
}

void CTransportSettings::setRecQualityMode (QString const & mode)
{
  m_d->m_recQualityMode = mode;
}

QString const & CTransportSettings::playMode () const
{
  return m_d->m_playMode;
}

QString const & CTransportSettings::recQualityMode () const
{
  return m_d->m_recQualityMode;
}
