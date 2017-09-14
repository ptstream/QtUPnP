#ifndef DIDL_ITEM_HPP
#define DIDL_ITEM_HPP

#include "using_upnp_namespace.hpp"
#include "upnp_global.hpp"
#include "status.hpp"
#include <QSharedDataPointer>
#include <QMultiMap>

START_DEFINE_UPNP_NAMESPACE

/*! Defines the properties map. */
typedef QMap<QString, QString> TMProps;

struct SDidlElemData;
struct SDidlItemData;

/*! \brief Holds information about the xml item tag for the Browse or Search action.
 *
 * The element is a set of properties and a value.
 * For example, the xml res element contains a set of properties (bitrate=19200, frequency=44100) and
 * the uri of the music track.
 * The xml dc:title tag has no property but a value, the title of the music track.
 *
 * For example the xml data of an CDidlElem is for the music track title.
 * \code
 * <dc:title>Jingle Bells</dc:title>
 *
 * QString title = CDidlItem::value (); // Returns "Jingle Bells".
 * \endcode
 *
 * A more complex example is the xml res tag.*
 * \code
 * <res duration= 0:02:03.000 bitrate=1600 sampleFrequency=44100 size=1988794 nrAudioChannels=2 protocolInfo=http-get:*:audio/mpeg>
 * http://192.168.0.77:50002/m/NDLNA/271292.mp3
 * </res>
 *
 * QString trackURI = CDidlItem::value () // returns "http://192.168.0.77:50002/m/NDLNA/271292.mp3".
 * unsigned . bitrate = CDidlItem::props ().value ("bitrate").toUInt ().
 * QString duration = CDidlItem::props ().value ("duration");
 *
 * \endcode
 *
 * \remark Use implicit Sharing QT technology.
 */
class CDidlElem
{
public :
  /*! Default constructor. */
  CDidlElem ();

  /*! Copy constructor. */
  CDidlElem (CDidlElem const & other);

  /*! Destructor. */
  ~CDidlElem ();

  /*! Equal operator. */
  CDidlElem& operator = (CDidlElem const & other);

  /*! Sets a list of properties.
   * \param props: The map of properties.
   */
  void setProps (TMProps const & props);

  /*! Sets the value.
   * \param value: The value of the element.
   */
  void setValue (QString const & value);

  /*! Returns the property map as a constant reference. */
  TMProps const & props () const;

  /*! Returns the value. */
  QString const & value () const;

  /*! Returns true if the value and the property map are empty. */
  bool isEmpty () const;

private :
  QSharedDataPointer<SDidlElemData> m_d; //!< Shared data pointer.
};

/*! \brief Holds information about the xml item tag of the xml DIDL-Lite tag server response
 * for the Browse or Search action.
 *
 * The elements of the item tag component are stored in a multimap.
 * The multimap key is the element name. e.g. dc:title, upnp:class...
 *
 * For example the xml tag item is :
 * \code
 * <item id="22$@273788" parentID="22$28811" restricted="1">
 * <dc:title>Rossini, Gioachino: Aragonese</dc:title>
 * <upnp:class>object.item.audioItem.musicTrack</upnp:class>
 * <dc:date>1997-01-01</dc:date>
 * <upnp:album>An Italian Songbook</upnp:album>
 * <upnp:albumArtist>Cecilia Bartoli</upnp:albumArtist>
 * <upnp:artist>Cecilia Bartoli</upnp:artist>
 * <dc:creator>Cecilia Bartoli</dc:creator>
 * <upnp:genre>Arias</upnp:genre>
 * <upnp:originalTrackNumber>1</upnp:originalTrackNumber>
 * <upnp:albumArtURI dlna:profileID="JPEG_TN">http://192.168.0.77:50002/transcoder/jpegtnscaler.cgi/ebdart/273788.jpg</upnp:albumArtURI>
 * <res protocolInfo="http-get:*:audio/x-flac:*" size="14217725" bitrate="67625" duration="0:03:30.000" nrAudioChannels="2" sampleFrequency="44100">http://192.168.0.77:50002/m/NDLNA/273788.flac</res>
  </item>
 * \endcode
 *
 * The CDidlItem represents the item tag. Each sub-item is a CDidlElem.
 * The CDidlItem multimap is required because the server can return more than one CDidlItem with the
 * same name. For example, Windows Media Player return several res item with different bitrates.
 *
 * The CDidlItem names are those defined in the UPnP/AV protocol. The prefix (upnp: or dc:) is the namespace.
 * \li upnp defines UPnP namespace.
 * \li dc defines the Dublin Core metadata representation (see http://dublincore.org/ and http://dublincore.org/documents/dces/).
 *
 * To facilitate the use, a set of functions can be use to retrieve the item metadata. All of these functions return an empty string or 0
 * if the CItemElem or the property is not present.
 * \code
 * QString uri (int index) const;
 * QString albumArtURI (int index) const;
 * QString title () const;
 * QString artist () const;
 * QString actor () const;
 * QString album () const;
 * QString albumArtist () const;
 * QString creator () const;
 * QString genre () const;
 * QString date () const;
 * QString duration (int index = 0) const;
 * QString protocolInfo (int index = 0) const;
 * QString composer () const;
 * unsigned originalTrackNumber () const;
 * unsigned bitrate (int index = 0) const;
 * unsigned nrAudioChannels (int index = 0) const;
 * unsigned size (int index = 0) const;
 * unsigned sampleFrequency (int index = 0) const;
 * QString description () const;
 * QString rights () const;
 * QString language () const;
 * QString publisher () const;
 * QString contributor () const;
 * QString relation () const;
 * QString coverage () const;
 * QString format () const;
 * QString identifier () const;
 * QString source () const;
 * QString subject () const;
 * \endcode
 */
class CDidlItem
{
public:
  /*! Defines the type of xml item element.
   * See http://upnp.org/specs/av/UPnP-av-ContentDirectory-Service.pdf.
   */
  enum EType {
               Unknown,                   //!< unknown
               ImageItem,                 //!< object.item.imageItem
               Photo,                     //!< object.item.imageItem.photo
               AudioItem,                 //!< bject.item.audioItem
               MusicTrack,                //!< object.item.audioItem.musicTrack
               AudioBroadcast,            //!< object.item.audioItem.audioBroadcast
               AudioBook,                 //!< object.item.audioItem.audioBook
               VideoItem,                 //!< object.item.videoItem
               Movie,                     //!< object.item.videoItem.movie
               VideoBroadcast,            //!< object.item.videoItem.videoBroadcast
               MusicVideoClip,            //!< object.item.videoItem.musicVideoClip
               TextItem,                  //!< object.item.textItem
               BookmarkItem,              //!< object.item.bookmarkItem
               EpgItem,                   //!< object.item.epgItem
               AudioProgram,              //!< object.item.epgItem.audioProgram
               VideoProgram,              //!< object.item.epgItem.videoProgram

               PlaylistItem,              //!< object.item.playlistItem
               Container,                 //!< object.container
               PlaylistContainer,         //!< object.container.playlistContainer
               MusicAlbum,                //!< object.container.album.musicAlbum
               PhotoAlbum,                //!< object.container.album.photoAlbum
               EpgContainer,              //!< object.container.epgContainer
               StorageSystem,             //!< object.container.storageSystem
               StorageVolume,             //!< object.container.storageVolume
               StorageFolder,             //!< object.container.storageFolder
               BookmarkFolder,            //!< object.container.bookmarkFolder
               Person,                    //!< object.container.person
               MusicArtist,               //!< object.container.person.musicArtist
               Genre,                     //!< object.container.genre
               MusicGenre,                //!< object.container.genre.musicGenre
               MovieGenre,                //!< object.container.genre.movieGenre
               AudioChannelGroup,         //!< object.container.channelGroup.audioChannelGroup
               VideoChannelGroup,         //!< object.container.channelGroup.videoChannelGroup

               LastType,                  //!< Just for dimensioning array or endding loop.
             };

  enum EStatus { SortRes = 0x00000001, //!< The xml res elements will be sorted by resolution or . bitrate.
                 SortAlbumArt = 0x00000002 //!< The xml albumArtURI elements will be sorted by resolution or . bitrate.
               };

  /*! Playlist format. Actually only M3u and M3u8 can be used. */
  enum EPlaylistFormat { M3u, M3u8, Wpl, Xspl };

  /*! Type definition of a playlist element. */
  typedef QPair<CDidlItem, int> TPlaylistElem;

  /*! Default constructor. */
  CDidlItem ();

  /*! Copy constructor. */
  CDidlItem (CDidlItem const & other);

  /*! Destructor. */
  ~CDidlItem ();

  /*! Equal operator. */
  CDidlItem& operator = (CDidlItem const & other);

  /*! Inserts a new element in the multimap.
   * \param name : The name of element. e.g. dc:title, upnp:class
   * \param elem : The CDidlElem
   */
  void insert (QString const & name, CDidlElem const & elem);

  /*! Replaces the last inserted element named name in the multimap.
   * \param name : The name of element. e.g. dc:title, upnp:class
   * \param elem : The CDidlElem
   */
  void replace (QString const & name, CDidlElem const & elem);

  /*! Returns the value of the element named name.
   * \param name : The name of element. e.g. dc:title, upnp:class
   */
  CDidlElem value (QString const & name)  const;

  /*! Returns the value of an element property.
   * \param elemName : The element name. e.g. dc:title, upnp:class
   * \param propName : The property name. e.g id
   */
  QString value (QString const & elemName, QString const & propName) const;

  /*! Returns the values of the element named name.
   * \param name : The name of element. e.g. res, unpn:albumArtURI
   */
  QList<CDidlElem> values (QString const & name)  const;

  /*! Returns the multimap elements as a constant reference. */
  QMultiMap<QString, CDidlElem> const & elems () const;

  /*! Returns the multimap elements as a reference. */
  QMultiMap<QString, CDidlElem>& elems ();

  /*! Returns the type of item components. */
  EType type () const;

  /*! Returns true if the multimap of elements is empty. Otherwize false. */
  bool isEmpty () const;

  /*! Returns true if the item element is a container.
     A container has a type less equal videoProgram.
  */
  bool isContainer () const;

  /*! Returns the list of res uri elements.
   * The list is sorted by resolution, the better resolution is first,
   * if the static status has SortRes bit.
   */
  QStringList uris () const;

  /*! Returns res uri element.
   * \param index: The index of res element.
  */
  QString uri (int index) const;

  /*! Returns the list of albumArtURIs.
   * The list is sorted by resolution. The better resolution is first,
   * if the static status has SortAlbumArt bit.
   */
  QStringList albumArtURIs () const;

  /*! Returns AlbumArtURI uri element.
   * \param index: The index of albumArtURI element.
  */
  QString albumArtURI (int index) const;

  /*! Returns the element in form of DIDL_Lite xml tag. */
  QString didl () const;

  /*! Sorts the res elements by resolution or bitrate.
   * \return The number of elements.
   */
  int sortResElems ();

  /*! Sorts the albumArtURI elements by resolution.
   * \return The number of elements.
   */
  int sortAlbumArtURIs ();

  /*! Clear all content. */
  void clear ();

  /*! Returns the title. */
  QString title () const;

  /*! Returns the artists. */
  QString artist () const;

  /*! Returns the actor. */
  QString actor () const;

  /*! Returns the album. */
  QString album () const;

  /*! Returns the albumArtist. */
  QString albumArtist () const;

  /*! Returns the albumArtist. */
  QString creator () const;

  /*! Returns the genre. */
  QString genre () const;

  /*! Returns the date. */
  QString date () const;

  /*! Returns the duration. */
  QString duration (int index = 0) const;

  /*! Returns the protocol information. */
  QString protocolInfo (int index = 0) const;

  /*! Returns the composer. */
  QString composer () const;

  /*! Returns the originalTrackNumber. */
  unsigned originalTrackNumber () const;

  /*! Returns the bitrate. */
  unsigned bitrate (int index = 0) const;

  /*! Returns the nrAudioChannels. */
  unsigned nrAudioChannels (int index = 0) const;

  /*! Returns the size. */
  unsigned size (int index = 0) const;

  /*! Returns the sampleFrequency. */
  unsigned sampleFrequency (int index = 0) const;

  /*! Returns the description. */
  QString description () const;

  /*! Returns the rights . */
  QString rights () const;

  /*! Returns the language . */
  QString language () const;

  /*! Returns the publisher . */
  QString publisher () const;

  /*! Returns the contributor . */
  QString contributor () const;

  /*! Returns the relation . */
  QString relation () const;

  /*! Returns the coverage . */
  QString coverage () const;

  /*! Returns the format . */
  QString format () const;

  /*! Returns the identifier . */
  QString identifier () const;

  /*! Returns the source . */
  QString source () const;

  /*! Returns the subject . */
  QString subject () const;

  /*! Return the static status as a constant reference. */
  static CStatus const & status () { return m_status; }

  /*! Returns an UTF-8 encoded copy of not coded in format UTF-8.
   * not coded <, >, ", ' are percent encoded.
   */
  static QString toPercentEncodeing (QByteArray const & notCoded);

  /*! Returns true if the item element is a container. A container has a type less equal videoProgram.
   * \param type: Container type. See CDidlItem::EType.
  */
  static bool isContainer (EType type);

  /*! Returns the playlist data.
   * \param elems: The playlist elements.
   * \param format: The format.
   * \return The number of items in the playlist.
   */
   static QByteArray buildsPlaylist (QList<TPlaylistElem> const & elems, EPlaylistFormat format);

  /*! Returns the human readable of the element. */
  QStringList dump () const;

private :
  typedef QPair<unsigned, CDidlElem> TQE; //!< Pair of quality and CDidlElem. Used to sort elem;

  /*! Returns the values for a tag. e.g. value of <res> or <albumArtURI>. */
  QStringList stringValues (char const * tag) const;

  /*! Returns the data in M3u format. */
  static QByteArray m3u (QList<TPlaylistElem> const & playlistElems);

  /*! Returns the data in M3u8 format. */
  static QByteArray m3u8 (QList<TPlaylistElem> const & playlistElems);

private:
  QSharedDataPointer<SDidlItemData> m_d; //!< Shared data pointer.
  static CStatus m_status; //!< The static status for sorting.
};

} // Namespace

#endif // DIDL_ITEM_HPP
