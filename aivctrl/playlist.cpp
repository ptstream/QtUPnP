#include "playlist.hpp"
#include "xmlhplaylist.hpp"
#include "serverscanner.hpp"
#include "helper.hpp"
#include "../upnp/contentdirectory.hpp"
#include "../upnp/helper.hpp"
#include <QXmlStreamWriter>
#include <QProgressDialog>

USING_UPNP_NAMESPACE

QString const g_fileSuffix = ".xspf";

struct SPlaylistData : public QSharedData
{
  SPlaylistData () {}

  bool m_changed = false;
  QList<CDidlItem> m_items;
  int m_type = 0;
};

CPlaylist::CPlaylist () : m_d (new SPlaylistData)
{
}

CPlaylist::CPlaylist (EType type) : m_d (new SPlaylistData)
{
  m_d->m_type = type;
}

CPlaylist::CPlaylist (CPlaylist const & rhs) : m_d (rhs.m_d)
{
}

CPlaylist& CPlaylist::operator = (CPlaylist const & rhs)
{
  if (this != &rhs)
  {
    m_d.operator = (rhs.m_d);
  }

  return *this;
}

CPlaylist::~CPlaylist ()
{
}

QList<QtUPnP::CDidlItem> const & CPlaylist::items () const
{
  return m_d->m_items;
}

QList<QtUPnP::CDidlItem>& CPlaylist::items ()
{
  return m_d->m_items;
}

CPlaylist::EType CPlaylist::type () const
{
  return static_cast<EType>(m_d->m_type);
}

bool CPlaylist::changed () const
{
  return m_d->m_changed;
}

void CPlaylist::setItems (QList<QtUPnP::CDidlItem> const & items)
{
  m_d->m_items   = items;
  m_d->m_changed = true;
}

void CPlaylist::addItems (QList<QtUPnP::CDidlItem> const & items)
{
  m_d->m_items  += items;
  m_d->m_changed = true;
}

void CPlaylist::addItem (QtUPnP::CDidlItem const & item)
{
  m_d->m_items  += item;
  m_d->m_changed = true;
}

void CPlaylist::remItems (QStringList const & ids)
{
  int cIDs = ids.size ();
  if (cIDs != 0)
  {
    int cItems = m_d->m_items.size ();
    if (cIDs == cItems)
    { // Clear all.
      m_d->m_items.clear ();
    }
    else
    {
      QVector<int> indices; // Indices int the playlist.
      indices.reserve (cIDs);

      QList<QtUPnP::CDidlItem>::const_iterator begin = m_d->m_items.cbegin ();
      QList<QtUPnP::CDidlItem>::const_iterator end   = m_d->m_items.cend ();
      if (cItems > 100)
      { // More than 100 elements in the playlist. Use a hash coding table to speed up.
        QHash<QString, int> items;
        for (QList<QtUPnP::CDidlItem>::const_iterator it = begin; it != end; ++it)
        {
          items.insert ((*it).itemID (), it - begin);
        }

        for (QString const & id : ids)
        {
          if (items.contains (id))
          {
            indices.push_back (items.value (id));
          }
        }
      }
      else
      { // Small playlist. Use brut force.
        for (QString const & id : ids)
        {
          for (QList<QtUPnP::CDidlItem>::const_iterator it = begin; it != end; ++it)
          {
            if ((*it).itemID () == id)
            {
              indices.push_back (it - begin);
            }
          }
        }
      }

      // Sort indices to remove elements in reverse order.
      std::sort (indices.begin (), indices.end (), std::greater<int>());
      for (int index : indices)
      {
        m_d->m_items.removeAt (index);
      }
    }

    m_d->m_changed = true;
  }
}

void CPlaylist::setType (EType type)
{
  m_d->m_type = type;
}

void CPlaylist::setChanged (bool changed)
{
  m_d->m_changed = changed;
}

bool CPlaylist::isEmpty () const
{
  return m_d->m_items.isEmpty ();
}

CPlaylist::EType CPlaylist::favoritesType (QtUPnP::CDidlItem const & item)
{
  CPlaylist::EType type;
  switch (item.type ())
  {
    case CDidlItem::AudioItem :
    case CDidlItem::MusicTrack :
    case CDidlItem::AudioBroadcast :
    case CDidlItem::AudioBook :
    case CDidlItem::AudioProgram :
      type = CPlaylist::AudioFavorites;
      break;

    case CDidlItem::ImageItem :
    case CDidlItem::Photo :
      type = CPlaylist::ImageFavorites;
      break;

    case CDidlItem::VideoItem :
    case CDidlItem::Movie :
    case CDidlItem::VideoBroadcast :
    case CDidlItem::MusicVideoClip :
    case CDidlItem::VideoProgram :
      type = CPlaylist::VideoFavorites;
      break;

    default :
      type = CPlaylist::Unknown;
      break;
  }

  return type;
}

CPlaylist::EType CPlaylist::favoritesType (QList<QtUPnP::CDidlItem> const & items)
{
  CPlaylist::EType type = CPlaylist::Unknown;
  if (!items.isEmpty ())
  {
    type = favoritesType (items.first ());
  }

  return type;
}

CPlaylist::EType CPlaylist::playlistType (QtUPnP::CDidlItem const & item)
{
  CPlaylist::EType type = favoritesType (item);
  switch (type)
  {
    case CPlaylist::AudioFavorites :
      type = CPlaylist::Audio;
      break;

    case CPlaylist::ImageFavorites :
      type = CPlaylist::Image;
      break;

    case CPlaylist::VideoFavorites :
      type = CPlaylist::Video;
      break;

    default :
      type = CPlaylist::Unknown;
      break;
  }

  return type;
}

CPlaylist::EType CPlaylist::playlistType (QList<QtUPnP::CDidlItem> const & items)
{
  CPlaylist::EType type = favoritesType (items);
  if (!items.isEmpty ())
  {
    type = playlistType (items.first ());
  }

  return type;
}

void CPlaylist::saveItem (QXmlStreamWriter& stream, CDidlItem const & item)
{
  auto xmlWriteString  = [&stream] (QString const & name, QString value)
  {
    if (!value.isEmpty ())
    {
      stream.writeStartElement (name);
      stream.writeCharacters (value);
      stream.writeEndElement ();
    }
  };

  auto xmlWriteInt64 = [xmlWriteString, &stream] (QString const & name, long long value)
  {
    if (value != 0)
    {
      xmlWriteString (name, QString::number (value));
    }
  };

 // .. track
  stream.writeStartElement ("track");

  xmlWriteString ("location", item.uri (0));         // ... m_uri. Warning '%' encoded
  xmlWriteString ("title",    item.title ());        // ... title
  xmlWriteString ("album",    item.album ());        // ... album
  xmlWriteString ("image",    item.albumArtURI (0)); // ... image. Warning '%' encoded

  long long duration = ::timeToMS (item.duration ());
  xmlWriteInt64 ("duration", duration);     // ... duration

  // Metadata. DIDL-LITE form.
  QString didlLite = item.didl (false);
  stream.writeStartElement ("meta"); // ... meta
  stream.writeAttribute ("rel", "http://www.upnp.org/schemas/av/didl-lite.xsd");
  stream.writeCharacters (didlLite);
  stream.writeEndElement (); // ... meta

  stream.writeStartElement ("extension"); // ... Extension
  stream.writeAttribute ("application", APPURN);
  xmlWriteString ("aivctrl:upnpID", item.id ()); // .... aivctrl:upnpID
  stream.writeEndElement (); // ... Extension

  stream.writeEndElement (); // .. track
}

void CPlaylist::saveItems (QXmlStreamWriter& stream) const
{
  for (CDidlItem const & item : m_d->m_items)
  {
    saveItem (stream, item);
  }
}

void CPlaylist::save (QString const & name) const
{
  if (!name.isEmpty () && m_d->m_changed)
  {
    QDir    dir      = CPlaylist::dir ();
    QString filePath = dir.absoluteFilePath (name + g_fileSuffix);
    QFile   file (filePath);
    if (file.open (QIODevice::WriteOnly | QIODevice::Text))
    {
      QXmlStreamWriter stream (&file);
      stream.setAutoFormatting (true);
      stream.writeStartDocument ();

      // playlist
      stream.writeStartElement ("playlist");
      stream.writeAttribute ("version", "1");
      stream.writeAttribute ("xmlns", "http://xspf.org/ns/0/");

      // . Title
      stream.writeStartElement ("title");
      stream.writeCharacters (name);
      stream.writeEndElement (); // . Title

      stream.writeStartElement ("extension"); // . Extension
      stream.writeAttribute ("application", APPURN);
      stream.writeStartElement ("aivctrl:playlistType"); // .. aivctrl:playlistType
      stream.writeCharacters (QString::number (type ()));
      stream.writeEndElement (); // .. aivctrl:playlistType

      stream.writeStartElement ("aivctrl:nrTracks"); // .. aivctrl:nrTracks
      stream.writeCharacters (QString::number (m_d->m_items.size ()));
      stream.writeEndElement (); // .. aivctrl:nrTracks

      stream.writeEndElement (); // . Extension

      // . Track list
      stream.writeStartElement ("trackList");
      saveItems (stream);
      stream.writeEndElement (); // . trackList
      stream.writeEndElement (); // playlist
      stream.writeEndDocument ();
    }
  }
}

void CPlaylist::simplify ()
{
  QList<CDidlItem>& items = this->items ();
  for (CDidlItem& item : items)
  {
    QMultiMap<QString, CDidlElem>& elems       = item.elems ();
    QString const                  elemNames[] = { "item", "res", "upnp:albumArtURI" };
    for (unsigned k = 0; k < sizeof (elemNames) / sizeof (QString); ++k)
    {
      QList<CDidlElem> checkedElems = elems.values (elemNames[k]);
      bool             rem          = false;
      for (int i = 0, count = checkedElems.size (); i < count && !rem; ++i)
      {
        for (int j = i + 1; j < count; ++j)
        {
          QString const & vi = checkedElems[i].value ();
          QString const & vj = checkedElems[j].value ();
          if (vi == vj)
          {
            int si = checkedElems[i].props ().size ();
            int sj = checkedElems[j].props ().size ();
            if (si <= sj)
            {
              rem = true;
              elems.remove (elemNames[k], checkedElems[i]);
              break;
            }
          }
        }
      }
    }
  }
}

bool CPlaylist::restore (QString const & name)
{
  bool success = false;
  if (!name.isEmpty ())
  {
    QDir    dir      = CPlaylist::dir ();
    QString filePath = dir.absoluteFilePath (name + g_fileSuffix);
    QFile   file (filePath);
    if (file.open (QIODevice::ReadOnly | QIODevice::Text))
    {
      QByteArray    data = file.readAll ();
      CXmlHPlaylist h (*this);
      success = h.parse (data);
      if (success)
      {
        simplify ();
      }
    }
  }

  return success;
}

QDir CPlaylist::dir ()
{
  QDir    dir;
  QString folder = appDataDirectory ();
  if (!folder.isEmpty ())
  {
    dir.setPath (folder);
    if (!dir.exists ("playlists"))
    {
      dir.mkdir ("playlists");
    }

    dir.cd ("playlists");
  }

  return dir;
}

void CPlaylist::rowsMoved (int start, int end, int row)
{
  if (start >= 0 && end <= m_d->m_items.size () - 1)
  {
    QList<CDidlItem> items;
    items.reserve (end - start + 1);
    for (int i = start; i <= end; ++i)
    {
      items << m_d->m_items.takeAt (start);
    }

    if (row > end)
    {
      row -= end - start + 1;
    }
    else
    {
      row -= start;
    }

    for (CDidlItem const & item : items)
    {
      m_d->m_items.insert (row, item);
      ++row;
    }

    m_d->m_changed = true;
  }
}

bool CPlaylist::isFavorite () const
{
  return isFavorite (static_cast<EType>(m_d->m_type));
}

bool CPlaylist::contains (QtUPnP::CDidlItem const & didlItem) const
{
  bool            contain = false;
  QString const & id      = didlItem.id ();
  for (CDidlItem const & item : m_d->m_items)
  {
    if (item.id () == id)
    {
      contain = true;
      break;
    }
  }

  return contain;
}

bool CPlaylist::isFavorite (EType type)
{
  return type == AudioFavorites || type == ImageFavorites || type == VideoFavorites;
}

void savePlaylists (QMap<QString, CPlaylist> const & playlists)
{
  QDir        dir   = CPlaylist::dir ();
  QStringList files = dir.entryList (QStringList ("*" + g_fileSuffix));
  for (QString& file : files)
  {
    file.truncate (file.length () - g_fileSuffix.length ());
  }

  for (QMap<QString, CPlaylist>::const_iterator it = playlists.cbegin (), end = playlists.cend (); it != end; ++it)
  {
    CPlaylist const & playlist = it.value ();
    if (!playlist.isEmpty ())
    {
      QString const & name = it.key ();
      playlist.save (name);
      files.removeOne (name);
    }
  }

  for (QString const & file : files)
  {
    QString filePath = dir.absoluteFilePath (file + g_fileSuffix);
    QFile::remove (filePath);
  }
}

void restorePlaylists (QMap<QString, CPlaylist>& playlists)
{
  QDir        dir   = CPlaylist::dir ();
  QStringList files = dir.entryList (QStringList ("*" + g_fileSuffix));
  for (QString& file : files)
  {
    file.truncate (file.length () - g_fileSuffix.length ());
    CPlaylist playlist;
    playlist.restore (file);
    CPlaylist::EType type = playlist.type ();
    if (playlist.isFavorite ())
    {
      for (QMap<QString, CPlaylist>::iterator it = playlists.begin (), end = playlists.end (); it != end; ++it)
      {
        CPlaylist& exitingPlaylist = *it;
        if (exitingPlaylist.type () == type)
        {
          exitingPlaylist = playlist;
        }
      }
    }
    else
    {
      playlists.insert (file, playlist);
    }
  }
}

int playlistCount (QMap<QString, CPlaylist>& playlists, CPlaylist::EType type)
{
  int count = 0;
  for (QMap<QString, CPlaylist>::const_iterator it = playlists.cbegin (), end = playlists.cend (); it != end; ++it)
  {
    CPlaylist const & playlist = it.value ();
    if (playlist.type () == type)
    {
      ++count;
    }
  }

  return count;
}

CPlaylist* favorite (QMap<QString, CPlaylist>& playlists, CPlaylist::EType type)
{
  CPlaylist* playlist = nullptr;
  if (CPlaylist::isFavorite (type))
  {
    for (QMap<QString, CPlaylist>::iterator it = playlists.begin (), end = playlists.end (); it != end; ++it)
    {
      playlist = &(it.value ());
      if (playlist->type () == type)
      {
        break;
      }
    }
  }

  return playlist;
}

bool isFavorite (QMap<QString, CPlaylist>& playlists, QtUPnP::CDidlItem const & item)
{
  bool favorite = false;
  for (QMap<QString, CPlaylist>::iterator it = playlists.begin (), end = playlists.end (); it != end; ++it)
  {
    CPlaylist const & playlist = it.value ();
    if (playlist.isFavorite () && playlist.contains (item))
    {
      favorite = true;
      break;
    }
  }

  return favorite;
}




