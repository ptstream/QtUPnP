#ifndef PIXMAPCACHE_HPP
#define PIXMAPCACHE_HPP  1

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"
#include <QHash>
#include <QPixmap>
#include <QDateTime>

START_DEFINE_UPNP_NAMESPACE

/*! \brief Provides the mechanism to speed up the thumbnail display.
 *
 * Suppose we want to show a music container with a lot of tracks.
 * For each track, to show the thumbnail, the control point asks the data of the thumbnail using
 * the albumArtURI CDidlElem. This take time. The pixmap cache store in memory the pixmap data, and
 * retreive it quickly from its URI.
 * A mechanism is set up to store only pixmaps that are different. For this, CPixmapCache use the md5
 * of the pixmap data.
 *
 * setMaxEntries and setEntriesToRemove must be called once after the creation of this object.
 * According to the values entered, the entries to remove must be ajusted.
 * By default, the cache as an unlimited size.
 *
 * An example with 10472 music tracks:
 * \li m_md5s size is 10472.
 * \li m_dates size is 10472.
 * \li m_pixmaps size is 471.
 *
 * The functions pixmaps, md5s, dates and setPixmaps, setMd5s, setDates can be used to save on disk and load
 * from the disk the cache.
 * Simple iterator can be used to enumerate the elements of the hash table. For example:
 * \code
 * CPixmapCache cache;
 * QHash<QString, QPixmap> const & pxms = cache.pixmaps ();
 * for (QHash<QString, QPixmap>::const_iterator it = pxms.begin (), end = pxms.end (); it != end; ++it)
 * {
 *   QString const & md5   = it.key;
 *   QString const & bytes = it.value ();
 *   ******** Write md5 and bytes *************
 * }
 * ******* Make same thing for dates and md5s.
 * \endcode
 */
class CPixmapCache
{
public :
  /*! Default constructor. */
  CPixmapCache ();

  /*! Sets the maximum of entries in the cache. */
  void setMaxEntries (int cEntries);

  /*! Sets the number of entries to remove if the cache is full. */
  void setEntriesToRemove (int cEntries);

  /*! Returns the maximum of entries in the cache. */
  int maxEntries () const { return m_cMaxEntry; }

  /*! Returns the number of entries to remove if the cache is full. */
  int entriesToRemove () const { return m_cEntryToRemove; }

  /*! Clear the cache. */
  void clear ();

  /*! Return true if the cache has changed. The cache changes:
   * \li clear function is called when the cache was not empty.
   * \li add function is called.
   * \li search function is called.
   *
   * This flag can be used, for example, to avoid disk writing when the application close and not use the cache.
   */
  bool hasChanged () const { return m_changed; }

  /*! Sets the pixmap hash table. */
  void setPixmaps (QHash<QString, QPixmap> const & pixmaps) { m_pixmaps = pixmaps; }

  /*! Sets the md5 hash table. */
  void setMd5s (QHash<QString, QString> const & md5s) { m_md5s = md5s; }

  /*! Sets the dates hash table. */
  void setDates (QHash<QString, qint64> const & dates) { m_dates = dates; }

  /*! Returns the pixmap hash table. */
  QHash<QString, QPixmap> const & pixmaps () const { return m_pixmaps; }

  /*! Returns the md5 hash table. */
  QHash<QString, QString> const & md5s () const { return m_md5s; }

  /*! Returns the dates hash table. */
  QHash<QString, qint64> const & dates () const { return m_dates; }

  /*! Adds a album art URI in the cache.
   * \param albumArtURI: The URI defined by albumArtURI CDidlElem.
   * \param pxmBytes: The bytes that compose the image.
   * \param iconSize: Resize the image according to iconSize. If iconSize is null, the image is not resize.
   * \return The Pixmap. A null pixmap can be returned.
   *
   * \remark Resizing the pixmap can be used to have same size pixmaps in, for example, a QListWidget
   * while in files, the sizes can be very different (e.g. not square).
   */
  QPixmap add (QString const & albumArtURI, QByteArray const & pxmBytes, QSize const & iconSize = QSize ());

  /*! Search an image.
   * \param albumArtURI: The URI defined by albumArtURI CDidlElem.
   * \return The Pixmap. If the uri is nor reached, a null pixmap is returned.
   */
  QPixmap search (QString const & albumArtURI);

private :
  bool m_changed = false; //!< Indicate if the cache has changed
  int m_cMaxEntry = -1; //!< The maximun of md5 stored in th cache.
  int m_cEntryToRemove = 0; //!< The number of md5 remove to make room is the cache is full.
  QHash<QString, QPixmap> m_pixmaps; //!< Hash table of uri and pixmap bytes.
  QHash<QString, QString> m_md5s; //!< Hash table of uri and md5.
  QHash<QString, qint64> m_dates; //!< Hash table of uri and date.
};

} // Namespace

#endif
