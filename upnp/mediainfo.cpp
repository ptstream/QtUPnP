#include "mediainfo.hpp"
#include "xmlhdidllite.hpp"

START_DEFINE_UPNP_NAMESPACE

struct SMediaInfoData : public QSharedData
{
  SMediaInfoData () {}
  SMediaInfoData (SMediaInfoData const & other);

  unsigned m_nrTracks = 0;
  QString m_mediaDuration;
  QString m_currentURI;
  QString m_currentURIMetaData;
  QString m_nextURI;
  QString m_nextURIMetaData;
  QString m_playMedium;
  QString m_recordMedium;
  QString m_writeStatus;
};

SMediaInfoData::SMediaInfoData (SMediaInfoData const & other) : QSharedData (other),
           m_nrTracks (other.m_nrTracks), m_mediaDuration (other.m_mediaDuration),
           m_currentURI (other.m_currentURI), m_currentURIMetaData (other.m_currentURIMetaData),
           m_nextURI (other.m_nextURI), m_nextURIMetaData (other.m_nextURIMetaData),
           m_playMedium (other.m_playMedium), m_recordMedium (other.m_recordMedium),
           m_writeStatus (other.m_writeStatus)
{
}

}//Namespace

USING_UPNP_NAMESPACE

CMediaInfo::CMediaInfo () : m_d (new SMediaInfoData)
{
}

CMediaInfo::CMediaInfo (CMediaInfo const & rhs) : m_d (rhs.m_d)
{
}

CMediaInfo& CMediaInfo::operator = (CMediaInfo const & rhs)
{
  if (this != &rhs)
  {
    m_d.operator = (rhs.m_d);
  }

  return *this;
}

CMediaInfo::~CMediaInfo ()
{
}

void CMediaInfo::setNrTracks (unsigned nrTracks)
{
  m_d->m_nrTracks = nrTracks;
}

void CMediaInfo::setMediaDuration (QString const & duration)
{
  m_d->m_mediaDuration = duration;
}

void CMediaInfo::setCurrentURI (QString const & uri)
{
  m_d->m_currentURI = uri;
}

void CMediaInfo::setCurrentURIMetaData (QString const & metadata)
{
 m_d->m_currentURIMetaData = metadata;
}

void CMediaInfo::setNextURI (QString const & uri)
{
  m_d->m_nextURI = uri;
}

void CMediaInfo::setNextURIMetaData (QString const & metadata)
{
  m_d->m_nextURIMetaData = metadata;
}

void CMediaInfo::setPlayMedium (QString const & medium)
{
  m_d->m_playMedium = medium;
}

void CMediaInfo::setRecordMedium (QString const & medium)
{
  m_d->m_recordMedium = medium;
}

void CMediaInfo::setWriteStatus (QString const & status)
{
  m_d->m_writeStatus = status;
}

unsigned CMediaInfo::nrTracks () const
{
  return m_d->m_nrTracks;
}

QString const & CMediaInfo::mediaDuration () const
{
  return m_d->m_mediaDuration;
}

QString const & CMediaInfo::currentURI () const
{
  return m_d->m_currentURI;
}

QString const & CMediaInfo::currentURIMetaData () const
{
  return m_d->m_currentURIMetaData;
}

QString const & CMediaInfo::nextURI () const
{
  return m_d->m_nextURI;
}

QString const & CMediaInfo::nextURIMetaData () const
{
  return m_d->m_nextURIMetaData;
}

QString const & CMediaInfo::playMedium () const
{
  return m_d->m_playMedium;
}

QString const & CMediaInfo::recordMedium () const
{
  return m_d->m_recordMedium;
}

QString const & CMediaInfo::writeStatus () const
{
 return m_d->m_writeStatus;
}

CDidlItem CMediaInfo::didlItem (QString const & metaData)
{
  CXmlHDidlLite h;
  return h.firstItem (metaData);
}

CDidlItem CMediaInfo::currentURIDidlItem () const
{
  return didlItem (m_d->m_currentURIMetaData);
}

CDidlItem CMediaInfo::nextURIDidlItem () const
{
  return didlItem (m_d->m_nextURIMetaData);
}

