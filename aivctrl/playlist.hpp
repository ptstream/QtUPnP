#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include <QDir>

namespace QtUPnP {
  class CDidlItem;
  class CControlPoint;
}

class SPlaylistData;
class QXmlStreamWriter;

/*! \brief This class is playlist manager.
 *
 * A playlist is a list of CDidlItem. CDidlItems can be add, remove and reoganize.
 * The file format is "xspf", they can used by VLC.
 */
class CPlaylist
{
public:
  /*! Type of playlist. Warning must be the same values and order than CMyDeviceBrowserItem::Etype. */
  enum EType { AudioContainer, //!< Not used bit important
               AudioFavorites, //!< Audio favorites
               Audio,          //!< Audio user playlists
               ImageContainer, //!< Not used bit important
               ImageFavorites, //!< Image favorites
               Image,          //!< Audio user playlists
               VideoContainer, //!< Not used bit important
               VideoFavorites, //!< Video favorites
               Video,          //!< Video user playlists
               Unknown = 255,
             };

  /*! Default constructor. */
  CPlaylist ();

  /*! Constructor. */
  CPlaylist (EType type);

  /*! Copy constructor. */
  CPlaylist (CPlaylist const & rhs);

  /*! Destructor. */
  ~CPlaylist ();

  /*! Equal operator. */
  CPlaylist& operator = (CPlaylist const & rhs);

  /*! Returns the items list. */
  QList<QtUPnP::CDidlItem> const & items () const;

  /*! Returns the items list. */
  QList<QtUPnP::CDidlItem>& items ();

  /*! Returns true if the playlist contains the didlItem identifier. */
  bool contains (QtUPnP::CDidlItem const & didlItem) const;

  /*! Returns the type. */
  EType type () const;

  /*! Returns the flag changed. */
  bool changed () const;

  /*! Replaces all existing items by items. */
  void setItems (QList<QtUPnP::CDidlItem> const & items);

  /*! Add items at the existing items. */
  void addItems (QList<QtUPnP::CDidlItem> const & items);

  /*! Add item at existing items. */
  void addItem (QtUPnP::CDidlItem const & item);

  /*! Removes items with id in ids. */
  void remItems (QStringList const & ids);

  /*! Sets the type. */
  void setType (EType type);

  /*! Changes the changed flag. */
  void setChanged (bool changed = true);

  /*! Returns if the playlist has no items. */
  bool isEmpty () const;

  /*! Save the item playlist. */
  void saveItems (QXmlStreamWriter& stream) const;

  /*! Save the playlist. See savePlaylists. */
  void save (QString const & name) const;

  /*! Restore the playlist. See restorePlaylists. */
  bool restore (QString const & name);

  /*! Simplifies the playlist. This function keeps only one version of each item.
   * More than one version can exist after read xspf file.
   */
  void simplify ();

  /*! Reorganizes the playlist. */
  void rowsMoved (int start, int end, int row);

  /*! Returns if the playlist is a favorite playlist. */
  bool isFavorite () const;

  /*! Returns the favorites playlist type from item. */
  static EType favoritesType (QtUPnP::CDidlItem const & item);

  /*! Returns the favorites playlist type from item list.
   * In fact, the fuction returns the type of te first item.
   */
  static EType favoritesType (QList<QtUPnP::CDidlItem> const & items);

  /*! Returns the playlist type from item. */
  static EType playlistType (QtUPnP::CDidlItem const & item);

  /*! Returns the playlist type from items.
   * In fact, the fuction returns the type of te first item.
   */
  static EType playlistType (QList<QtUPnP::CDidlItem> const & items);

  /*! Save the items. */
  static void saveItem (QXmlStreamWriter& stream, QtUPnP::CDidlItem const & item);

  /*! Return the playlist directory. */
  static QDir dir ();

  /*! Returns if the playlist is a favorite playlist. */
  static bool isFavorite (EType type);

private:
  QSharedDataPointer<SPlaylistData> m_d; // Data
};

/*! Saves the playlists. */
void savePlaylists (QMap<QString, CPlaylist> const & playlists);

/*! Retores the playlists. */
void restorePlaylists (QMap<QString, CPlaylist>& playlists);

/*! Returns the number of playlists from a type. */
int playlistCount (QMap<QString, CPlaylist>& playlists, CPlaylist::EType type);

/*! Returns the playlist favorite address from a type. */
CPlaylist* favorite (QMap<QString, CPlaylist>& playlists, CPlaylist::EType type);

/*! Returns true if didlItem is in favorites. */
bool isFavorite (QMap<QString, CPlaylist>& playlists, QtUPnP::CDidlItem const & item);

#endif // PLAYLIST_HPP
