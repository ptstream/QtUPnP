#include "devicepixmap.hpp"

START_DEFINE_UPNP_NAMESPACE

/*! \brief Internal structure of CDevicePixmap. */
struct SDevicePixmapData : public QSharedData
{
  SDevicePixmapData () {}
  SDevicePixmapData (SDevicePixmapData const & other);

  QString m_url; //!< url of the pixmap.
  QString m_mimeType; //!< The mime type.
  int m_width = 0, m_height = 0, m_depth = 0; //!< The width, height and depth.
};

SDevicePixmapData::SDevicePixmapData (SDevicePixmapData const & other) :  QSharedData (other),
           m_url (other.m_url), m_mimeType (other.m_mimeType), m_width (other.m_width), m_height (other.m_height),
           m_depth (other.m_depth)
{
}

}//Namespace

USING_UPNP_NAMESPACE

CDevicePixmap::CDevicePixmap () : m_d (new SDevicePixmapData)
{
}

CDevicePixmap::CDevicePixmap (CDevicePixmap const & other) : m_d (other.m_d)
{
}

CDevicePixmap::~CDevicePixmap ()
{
}

CDevicePixmap& CDevicePixmap::operator = (CDevicePixmap const & other)
{
  if (this != &other)
  {
    m_d.operator = (other.m_d);
  }

  return *this;
}

void CDevicePixmap::setUrl (QString const & url)
{
  m_d->m_url = url;
}

void CDevicePixmap::setMimeType (QString mimeType)
{
  mimeType.remove ("image/");
  mimeType        = mimeType.toLower ();
  m_d->m_mimeType = mimeType;
}

void CDevicePixmap::setWidth (int w)
{
  m_d->m_width = w;
}

void CDevicePixmap::setHeight ( int h)
{
  m_d->m_height = h;
}

void CDevicePixmap::setDepth (int d)
{
  m_d->m_depth = d;
}

QString const & CDevicePixmap::url () const
{
  return m_d->m_url;
}

QString const & CDevicePixmap::mimeType () const
{
  return m_d->m_mimeType;
}

int CDevicePixmap::width () const
{
  return  m_d->m_width;
}

int CDevicePixmap::height () const
{
  return m_d->m_height;
}

int CDevicePixmap::depth () const
{
  return m_d->m_depth;
}

int CDevicePixmap::preferedCriteria () const
{
  return preferedCriteria (width (), height (), depth ());
}

bool CDevicePixmap::hasMimeType (char const * mimeType) const
{
  QString desc (mimeType);
  desc = desc.toLower ();
  return desc == m_d->m_mimeType;
}

