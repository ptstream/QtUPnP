#include "positioninfo.hpp"
#include "xmlhdidllite.hpp"

START_DEFINE_UPNP_NAMESPACE

struct SPositionInfoData : public QSharedData
{
  SPositionInfoData () {}
  SPositionInfoData (SPositionInfoData const & other);

  unsigned m_track = 0;
  QString m_trackDuration;
  QString m_trackMetaData;
  QString m_trackURI;
  QString m_relTime;
  QString m_absTime;
  int m_relCount = 0;
  int m_absCount = 0;

};

SPositionInfoData::SPositionInfoData (SPositionInfoData const & other) : QSharedData (other),
           m_track (other.m_track), m_trackDuration (other.m_trackDuration),
           m_trackMetaData (other.m_trackMetaData), m_trackURI (other.m_trackURI),
           m_relTime (other.m_relTime), m_absTime (other.m_absTime),
           m_relCount (other.m_relCount), m_absCount (other.m_absCount)
{
}

}//Namespace

USING_UPNP_NAMESPACE

CPositionInfo::CPositionInfo () : m_d (new SPositionInfoData)
{
}

CPositionInfo::CPositionInfo (CPositionInfo const & rhs) : m_d (rhs.m_d)
{
}

CPositionInfo& CPositionInfo::operator = (CPositionInfo const & rhs)
{
  if (this != &rhs)
  {
    m_d.operator = (rhs.m_d);
  }

  return *this;
}

CPositionInfo::~CPositionInfo ()
{
}

void CPositionInfo::setTrack (unsigned track)
{
  m_d->m_track = track;
}

void CPositionInfo::setTrackDuration (QString const & trackDuration)
{
  m_d->m_trackDuration = trackDuration;
}

void CPositionInfo::setTrackMetaData (QString const & trackMetaData)
{
  m_d->m_trackMetaData = trackMetaData;
}

void CPositionInfo::setTrackURI (QString const & trackURI)
{
  m_d->m_trackURI = trackURI;
}

void CPositionInfo::setRelTime (QString const & relTime)
{
  m_d->m_relTime = relTime;
}

void CPositionInfo::setAbsTime (QString const & absTime)
{
  m_d->m_absTime = absTime;
}

void CPositionInfo::setRelCount (int relCount)
{
  m_d->m_relCount = relCount;
}

void CPositionInfo::setAbsCount (int absCount)
{
  m_d->m_absCount = absCount;
}

unsigned CPositionInfo::track () const
{
  return m_d->m_track;
}

QString const & CPositionInfo::trackDuration () const
{
  return m_d->m_trackDuration;
}

QString const & CPositionInfo::trackMetaData () const
{
  return m_d->m_trackMetaData;
}

QString const & CPositionInfo::trackURI () const
{
  return m_d->m_trackURI;
}

QString const & CPositionInfo::relTime () const
{
  return m_d->m_relTime;
}

QString const & CPositionInfo::absTime () const
{
  return m_d->m_absTime;
}

int CPositionInfo::relCount () const
{
  return m_d->m_relCount;
}

int CPositionInfo::absCount () const
{
  return m_d->m_absCount;
}


CDidlItem CPositionInfo::didlItem () const
{
  CXmlHDidlLite h;
  return h.firstItem (m_d->m_trackMetaData);
}
