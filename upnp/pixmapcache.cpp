
#include "pixmapcache.hpp"
#include <QCryptographicHash>
#include <QPainter>

USING_UPNP_NAMESPACE

CPixmapCache::CPixmapCache ()
{
}

void CPixmapCache::setMaxEntries (int cEntries)
{
  m_cMaxEntry = cEntries;
  if (cEntries < m_cEntryToRemove * 4 / 5)
  {
    m_cEntryToRemove = m_cMaxEntry / 5; // 20%
  }
}

void CPixmapCache::setEntriesToRemove (int cEntries)
{
  m_cEntryToRemove = cEntries;
  if (m_cEntryToRemove > m_cMaxEntry / 2)
  {
    m_cEntryToRemove = m_cMaxEntry / 2;
  }
}

void CPixmapCache::clear ()
{
  m_changed = !m_pixmaps.isEmpty () || !m_md5s.isEmpty ();
  if (m_changed)
  {
    m_pixmaps.clear ();
    m_md5s.clear ();
    m_dates.clear ();
  }
}

QPixmap CPixmapCache::add (QString const & albumArtURI, QByteArray const & pxmBytes,
                           QSize const & iconSize)
{
  typedef QPair<qint64, QString> TDate;

  QPixmap pxm;
  if (!pxmBytes.isEmpty ())
  {
    QCryptographicHash md5Engine (QCryptographicHash::Md5);
    md5Engine.addData (pxmBytes);
    QString md5 = md5Engine.result ().toHex ();
    if (m_md5s.contains (md5))
    {
      pxm                  = m_pixmaps.value (md5);
      m_dates[albumArtURI] = QDateTime::currentMSecsSinceEpoch ();
    }
    else
    {
      if (m_cMaxEntry != -1 && m_md5s.size () >= m_cMaxEntry)
      {
        QVector<TDate> dates;
        dates.reserve (m_dates.size ());
        for (QHash<QString, qint64>::const_iterator it = m_dates.begin (), end =  m_dates.end (); it != end; ++it)
        {
          QString const & uri  = it.key ();
          qint64          date = it.value ();
          dates.push_back (TDate (date, uri));
        }

        std::sort (dates.begin (), dates.end ());
        int cEntriesToRemove = std::min (m_cEntryToRemove, dates.size ());
        for (int iEntry = 0; iEntry < cEntriesToRemove; ++iEntry)
        {
          QString const & uri = dates[iEntry].second;
          QString const & md5 = m_md5s.value (uri);
          m_md5s.remove (uri);
          m_dates.remove (uri);
          m_pixmaps.remove (md5);
        }
      }

      m_md5s.insert (albumArtURI, md5);
      m_dates.insert (albumArtURI, QDateTime::currentMSecsSinceEpoch ());
      pxm.loadFromData (pxmBytes);
      if (!pxm.isNull ())
      {
        if (!iconSize.isNull ())
        {
          pxm = pxm.scaled (iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
          if (pxm.width () != pxm.height ())
          { // Center the pixmap.
            QPixmap resizedPxm (iconSize);
            resizedPxm.fill (QColor (0, 0, 0, 0));
            QPainter painter (&resizedPxm);
            int x = (iconSize.width () - pxm.width ()) / 2;
            int y = (iconSize.height () - pxm.height ()) / 2;
            painter.drawPixmap (x, y, pxm);
            painter.end ();
            pxm = resizedPxm;
          }
        }

        m_pixmaps.insert (md5, pxm);
      }
    }

    m_changed = true;
  }

  return pxm;
}

QPixmap CPixmapCache::search (QString const & albumArtURI)
{
  QPixmap                                 pxm;
  QHash<QString, QString>::const_iterator it = m_md5s.find (albumArtURI);
  if (it != m_md5s.end ())
  {
    QString key          = it.value ();
    pxm                  = m_pixmaps.value (key);
    m_dates[albumArtURI] = QDateTime::currentMSecsSinceEpoch ();
    m_changed            = true;
  }

  return pxm;
}

