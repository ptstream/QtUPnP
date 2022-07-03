#include "devicecaps.hpp"

#include <QStringList>

START_DEFINE_UPNP_NAMESPACE

/*! \brief Internal strucure for CDeviceCaps. */
struct SDeviceCapsData : public QSharedData
{
  SDeviceCapsData () {}
  SDeviceCapsData (SDeviceCapsData const & other);

  QStringList m_playMedias;
  QStringList m_recMedias;
  QStringList m_recQualityModes;
};

SDeviceCapsData::SDeviceCapsData (SDeviceCapsData const & other) : QSharedData (other),
          m_playMedias (other.m_playMedias),
          m_recMedias (other.m_recMedias),
          m_recQualityModes (other.m_recQualityModes)
{
}

}//Namespace

USING_UPNP_NAMESPACE

CDeviceCaps::CDeviceCaps () : m_d (new SDeviceCapsData)
{
}

CDeviceCaps::CDeviceCaps (CDeviceCaps const & rhs) : m_d (rhs.m_d)
{
}

CDeviceCaps& CDeviceCaps::operator = (CDeviceCaps const & rhs)
{
  if (this != &rhs)
  {
    m_d.operator = (rhs.m_d);
  }

  return *this;
}

CDeviceCaps::~CDeviceCaps ()
{
}

void CDeviceCaps::setPlayMedias (QStringList const & medias)
{
  m_d->m_playMedias = medias;
}

void CDeviceCaps::setRecMedias (QStringList const & medias)
{
  m_d->m_recMedias = medias;
}

void CDeviceCaps::setRecQualityModes (QStringList const & modes)
{
  m_d->m_recQualityModes = modes;
}

QStringList const & CDeviceCaps::playMedias () const
{
  return m_d->m_playMedias;
}

QStringList const & CDeviceCaps::recMedias () const
{
  return m_d->m_recMedias;
}

QStringList const & CDeviceCaps::recQualityModes () const
{
  return m_d->m_recQualityModes;
}
