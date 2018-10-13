#ifndef CONTENTDIRECTORYDBROWSER_HPP
#define CONTENTDIRECTORYDBROWSER_HPP

#include "listwidgetbase.hpp"
#include "../upnp/contentdirectory.hpp"
#include <QTimer>

namespace QtUPnP {
  class CControlPoint;
  class CPixmapCache;
}

/*! brief The CContentDirectoryBrowserItem class provides an item for use with the CContentDirectoryBrowser.
 * This class adds CDidlItem at QListWidgetItem used to display server content directory items,
 * playlists items, ...
 * It also manages the icon for each item.
 */
class CContentDirectoryBrowserItem : public QListWidgetItem
{
public :
  /*! The icon type. */
  enum EIconType { IconNotdefined, //!< Not define.
                   IConStandard, //!< The standard icon is used.
                   IconServer //!< The icon is provided by the server.
                 };

  /*! Default constructor. */
  CContentDirectoryBrowserItem (QListWidget* parent = nullptr, int type = Type);

  /*! The constructor with the CDidlItem. */
  CContentDirectoryBrowserItem (QtUPnP::CDidlItem const & didlItem, QListWidget* parent = nullptr, int type = Type);

  /*! The copy constructor. */
  CContentDirectoryBrowserItem (CContentDirectoryBrowserItem const & other);

  /*! Set the icon at undefined. */
  void resetIcon () { m_iconType = IconNotdefined; }

  /*! Returns the type of icon. */
  EIconType iconType () const { return m_iconType; }

  /*! Sets the type of icon. */
  void setIconType (EIconType type) { m_iconType = type; }

  /*! Returns the CDidlItem. */
  QtUPnP::CDidlItem const & didlItem () const { return m_didlItem; }

  /*! Udates the icon from the cache or from the server. */
  void updateIcon (QtUPnP::CControlPoint* cp, QtUPnP::CPixmapCache* cache, int iconIndex,
                   QStringList const & defaultPixmaps, QSize const & iconSize);

  /*! Update the tooltip. */
  void updateTooltip ();

  /*! Returns the standrad icon from the type of CDidlItem. */
  static QPixmap standardIcon (QtUPnP::CDidlItem const & didlItem, QStringList const & defaultPixmaps);

  /*! Changes the timeout to get icon from the server. */
  static void setAlbumArtURITimeout (int timeout) { CContentDirectoryBrowserItem::m_albumArtURITimeout = timeout; }

  /*! Returns the icon timeout. */
  static int albumArtURITimeout () { return CContentDirectoryBrowserItem::m_albumArtURITimeout; }

protected :
  QtUPnP::CDidlItem m_didlItem; //!< The item returned by browse action.
  EIconType m_iconType = IconNotdefined; //!< The tye of icon.
  static int m_albumArtURITimeout; //!< The timeout to retreave the icon from the server.
};

/*! \brief The CContentDirectoryBrowser class provides a CContentDirectoryBrowserItem list widget.
 *
 * This class is used to display server content directory, playlists, ...
 * The icons are updated with thumbnails provided the cache or by the server. Because update icons
 * can be very slow, a timer is responsible for updating icons. Using this method, it is possible
 * to stop at any time the update. For exemple when the widget is scrolled.
 */
class CContentDirectoryBrowser : public CListWidgetBase
{
  Q_OBJECT

public:
  /*! Default constructor. */
  CContentDirectoryBrowser (QWidget* parent = nullptr);

  /*! Sets the pixmap cache. */
  void setPixmapCache (QtUPnP::CPixmapCache* pixmapCache) { m_pixmapCache = pixmapCache; }

  /*! Sets the sort criteria.
   * Without criteria, CContentDirectoryBrowser show items in the order defined by the server.
   * It is possible to sort items from a value or a property. The criteria is similar at UPnP criteria.
   * To sort items by value:
   * \li Sort by titles, the criteria is "dc:title".
   * \li Sort bt creators, the criteria is "dc:creator".
   * \li Sort by genre, the criteria is "upnp:genre".
   * \li Sort by album, the criteria is "upnp:album".
   * \li Sort by date, the criteria is "dc:date".
   *
   * To sort items by a property:
   * \li Sort by size, the criteria is "res@size".
   * \li Sort by duration, the criteria is "res@duration".
   * \li Sort by bitrate, the criteria is "res@bitrate".
   */
  void setSortCriteria (QString const & criteria) { m_sortCriteria = criteria; }

  /*! Defines the direction of sorting. Must be CBrowseReply::ascending or CBrowseReply::descending. */
  void setSortDir (QtUPnP::CBrowseReply::ESortDir dir) { m_sortDir = dir; }

  /*! Returns the pixmap cache. */
  QtUPnP::CPixmapCache* pixmapCache () const { return m_pixmapCache; }

  /*! Returns the actual sort criteria. */
  QString const & sortCriteria () const { return m_sortCriteria; }

  /*! Returns the actual sort direction. */
  QtUPnP::CBrowseReply::ESortDir sortDir () const { return m_sortDir; }

  /*! Adds CContentDirectoryBrowserItem from the list of CDidlItems. */
  int addItems (QList<QtUPnP::CDidlItem> const & didlItems);

  /*! Adds from an id et a server. The arguments are identical at browse action. */
  int addItems (QString const & server, QString const & id,
                QtUPnP::CContentDirectory::EBrowseType type = QtUPnP::CContentDirectory::BrowseDirectChildren,
                QString filter = QString::null, int startingIndex = 0, int requestedCount = 0,
                QString const & sortCriteria = QString::null);

  /*! Starts the timer to update icons. */
  void startIconUpdateTimer ();

  /*! Sets bold an item with the uri equal at the argument. */
  void setBold (QString const & uri);

  /*! Sets the control point. */
  void setControlPoint (QtUPnP::CControlPoint* cp) { m_cp = cp; }

  /*! Return the index previous or next. */
  int nextIndex (bool forward, bool repeat, bool shuffle);

  /*! Returns the CDidlItem with the uri equal at the argument. */
  QtUPnP::CDidlItem didlItem (QString const & uri) const;

  /*! Resets the number of items selected. */
  void resetSelectedCount () { m_cSelected = 0; }

  /*! Decrement the number of items selected. */
  void decSelectedCount () { --m_cSelected; }

  /*! Increment the number of items selected. */
  void incSelectedCount ()  { ++m_cSelected; }

  /*! Returns the number of items selected. */
  int selectedCount () const { return m_cSelected; }

  /*! Block or release the update of the icons. */
  void blockIconUpdate (bool block) { m_blockIconUpdate = block; }

  /*! Returns the status blocked or not of the icons. */
  bool iconUpdateBlocked () const { return m_blockIconUpdate; }

  /*! Sets icon type at all items. */
  void setIconType (CContentDirectoryBrowserItem::EIconType type);

  /*! Stops the update of the icons. */
  static void stopIconUpdateTimer ();

protected slots :
  /*! The scroll bar position has changed. The update icon timer starts. */
  void scrollBarValueChanged (int);

  /*! Updates the icons. Called by the icon timer. */
  void updateIcon ();

  /*! Item selection has changed. */
  void itemSelectionChanged ();

  /*! The item will be played. Called by return key. */
  void startStream ();

protected :
  /*! The widget size has changed. The update icon timer starts. */
  virtual void resizeEvent (QResizeEvent*);

protected :
  QtUPnP::CPixmapCache* m_pixmapCache = nullptr; //!< The icon cache.
  QtUPnP::CControlPoint* m_cp = nullptr; //!< The control point.
  QString m_sortCriteria;  //!< The sort criteria.
  QtUPnP::CBrowseReply::ESortDir m_sortDir; //!< The sort direction.
  int m_cSelected = 0; //!< The current number of item selected.
  bool m_blockIconUpdate = false; //!< Block or release the update of icons.

  static QTimer m_iconUpdateTimer; //!< One timer for all CContentDirectoryBrowser
  static int const m_iconUpdateThresholdTime; //!< Start interval(500ms)
  static int const m_iconUpdateRepeatTime; //!< After start interval, reduce interval at 10ms.
  static CContentDirectoryBrowser* m_directoryBrowser; //!< CContentDirectoryBrowser for the current timer.
};


#endif // CONTENTDIRECTORYDBROWSER_HPP
