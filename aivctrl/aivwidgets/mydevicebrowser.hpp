#ifndef MYDEVICEBROWSER_HPP
#define MYDEVICEBROWSER_HPP

#include <QListWidget>

/*! \brief It is the item of CMyDeviceBrowser. */
 class CMyDeviceBrowserItem : public QListWidgetItem
{
public :
  /*! Item type. */
  enum EType { AudioContainer, //!< Audio container item.
               AudioFavorite, //!< Audio favorite item.
               AudioPlaylist, //!< Audio user playlist item.
               ImageContainer, //!< Image container item.
               ImageFavorite, //!< Image favorite item.
               ImagePlaylist, //!< Image user playlist item.
               VideoContainer, //!< Video container item.
               VideoFavorite, //!< Video favorite item.
               VideoPlaylist, //!< Video user playlist item.
             };

  /*! Default constructor. */
  CMyDeviceBrowserItem (QIcon const & icon, QString const & name, EType type);

  /*! Returns the type of the item. */
  EType type () const { return m_type; }

  /*! Sets the type of the item. */
  void setType (EType type) { m_type = type; }

  /*! Returns true if the item is a container. */
  bool isContainer () const;

  /*! Returns true if the item is a user playlist. */
  bool isUserPlaylist () const;

  /*! Returns true if the item is a favorite. */
  bool isFavorite () const;

  /*! Returns the default user playlist name from the item type. */
  QString defaultNewPlaylistName () const { return defaultNewPlaylistName (m_type); }

  /*! Returns favorite name from the item type. */
  QString favoritesPlaylistName () const { return favoritesPlaylistName (m_type); }

  /*! Returns the playlist name. */
  QString playlistName () const;

  /*! Set count at te item. */
  void setCount (int count);

  /*! Lessthan operator for sorting. */
  virtual bool operator < (QListWidgetItem const & other) const;

  /*! Returns the default user playlist name from the type. */
  static QString defaultNewPlaylistName (int type);

  /*! Returns the default favorite name from the type. */
  static QString favoritesPlaylistName (int type);

  /*! Remove count at the item title. */
  static int removeCount (QString& text);

  /*! Returns true if the item is a container. */
  static bool isContainer (EType type);

protected :
  EType m_type = AudioContainer; //!< The item type.
};

/*! \brief This class provides functionalities to manage playlists names.
 *
 * CMyDeviceBrowser contains automatically:
 * \li An item for playlist container for each type (audio, image, video).
 * \li An item for favorites for each type.
 * \li A set of user playlists for each type.
 */
class CMyDeviceBrowser : public QListWidget
{
  Q_OBJECT
public:
  /*! Default constructor. */
  CMyDeviceBrowser (QWidget* parent = nullptr);

  /*! Retranslates favorites and containers titles. */
  void retranslateSpecialItems ();

  /*! Creates default playlists. */
  void createDefaultPlaylists ();

  /*! Collapses or expands container items. */
  void setCollapsed (CMyDeviceBrowserItem::EType type, bool collapse);

  /*! Collapses or expands container items. */
  void setCollapsed (CMyDeviceBrowserItem* item, bool collapse);

  /*! Toggles collapse or expand container items. */
  bool collapseExpand (CMyDeviceBrowserItem* item);

  /*! Toggle collapse or expand container items. */
  bool isCollapsed (CMyDeviceBrowserItem* item);

  /*! Manages container double click. */
  void containerDoubleClicked (CMyDeviceBrowserItem* item);

  /*! Returns the item from its row. */
  CMyDeviceBrowserItem* item (int row) { return static_cast<CMyDeviceBrowserItem*>(this->QListWidget::item (row)); }

  /*! Update the name of the playlist. For playlists with same name, CMyDeviceBrowser add an index.
   * E.g. if the playlist named "new playlist" exists and if the another playlist is created with the
   * same name, CMyDeviceBrowser rename the new playlist in "new playlist-2" and maybe "new playlist-3",
   * "new playlist-4"...
   */
  QString updateName (QListWidgetItem* item);

  /*! Adds a new playlist name. */
  CMyDeviceBrowserItem* addItem (int type, QString const & name);

  /*! Returns a container item from an item. */
  CMyDeviceBrowserItem* containerItem (CMyDeviceBrowserItem* item);

  /*! Returns a container item from a type. */
  CMyDeviceBrowserItem* containerItem (CMyDeviceBrowserItem::EType type);

  /*! Returns the first item for a CMyDeviceBrowserItem::EType. */
  CMyDeviceBrowserItem* item (CMyDeviceBrowserItem::EType type);

  /*! Sets count at an item. */
  void setCount (CMyDeviceBrowserItem::EType type, int count);

  /*! Set count at a playlist name. */
  void setCount (QString const & name, int count);

  /*! Updates item count. */
  void updateContainerItemCount ();

public slots :
  /*! Removes a playlist. */
  void delKey ();

  /*! Renames a playlist. */
  void rename ();

protected slots :
  /*! Update the name of the playlist. */
  void commitData (QWidget *editor);

  /*! Generate item double click for key Return. */
  void keyReturn ();

signals :
  /*! A new playlist has been created. */
  void newPlaylist (QString const & name, int type);

  /*! A playlist has been renamed. */
  void renamePlaylist (QString const & name, QString const & newName);

  /*! A playlist has been memoved. */
  void removePlaylist (QString const & name);

private :
  QColor m_rootColor = QColor (0x0, 0x59, 0xc7, 30); //!< Container item color.
};

#endif // MYDEVICEBROWSER_HPP
