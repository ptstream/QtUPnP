#include "didlitem.hpp"
#include <QBuffer>
#include "QXmlStreamWriter"

START_DEFINE_UPNP_NAMESPACE

/*! \brief Internal structure of CDidlElem. */
struct SDidlElemData  : public QSharedData
{
  /*! Default constructor. */
  SDidlElemData () {}

  /*! Copy constructor. */
  SDidlElemData (SDidlElemData const & other);

  TMProps m_props; //!< The map of porperties.
  QString m_value; //!< The value.
};

SDidlElemData::SDidlElemData (SDidlElemData const & other) : QSharedData (other),
                          m_props (other.m_props), m_value (other.m_value)
{
}

/*! \brief Internal structure of CDidlItem. */
struct SDidlItemData : public QSharedData
{
  /*! Default constructor. */
  SDidlItemData () {}

  /*! Copy constructor. */
  SDidlItemData (SDidlItemData const & other);

  /*! The multimap of the item elements. */
  QMultiMap<QString, CDidlElem> m_elems;
};

SDidlItemData::SDidlItemData (SDidlItemData const & other) : QSharedData (other),
           m_elems (other.m_elems)
{
}

}//Namespace

USING_UPNP_NAMESPACE

// upnp::class element values.
static const char *s_typeTags[] = { "unknown",
                                    "object.item.imageItem",
                                    "object.item.imageItem.photo",
                                    "object.item.audioItem",
                                    "object.item.audioItem.musicTrack",
                                    "object.item.audioItem.audioBroadcast",
                                    "object.item.audioItem.audioBook",
                                    "object.item.videoItem",
                                    "object.item.videoItem.movie",
                                    "object.item.videoItem.videoBroadcast",
                                    "object.item.videoItem.musicVideoClip",
                                    "object.item.textItem",
                                    "object.item.bookmarkItem",
                                    "object.item.epgItem",
                                    "object.item.epgItem.audioProgram",
                                    "object.item.epgItem.videoProgram",

                                    "object.item.playlistItem",
                                    "object.container",
                                    "object.container.playlistContainer",
                                    "object.container.album.musicAlbum",
                                    "object.container.album.photoAlbum",
                                    "object.container.epgContainer",
                                    "object.container.storageSystem",
                                    "object.container.storageVolume",
                                    "object.container.storageFolder",
                                    "object.container.bookmarkFolder",
                                    "object.container.person",
                                    "object.container.person.musicArtist",
                                    "object.container.genre",
                                    "object.container.genre.musicGenre",
                                    "object.container.genre.movieGenre",
                                    "object.container.channelGroup.audioChannelGroup",
                                    "object.container.channelGroup.videoChannelGroup",
                                    ""
                                  };

CDidlElem::CDidlElem () : m_d (new SDidlElemData)
{
}

CDidlElem::CDidlElem (QString const & value) : m_d (new SDidlElemData)
{
  m_d->m_value = value;
}

CDidlElem::CDidlElem (CDidlElem const & other) : m_d (other.m_d)
{
}

CDidlElem::~CDidlElem ()
{
}

CDidlElem& CDidlElem::operator = (CDidlElem const & other)
{
  if (this != &other)
  {
    m_d.operator = (other.m_d);
  }

  return *this;
}

void CDidlElem::setProps (TMProps const & props)
{
  m_d->m_props = props;
}

void CDidlElem::addProp (QString const & name, QString const & value)
{
  m_d->m_props.insert (name, value);
}

void CDidlElem::setValue (QString const & value)
{
  m_d->m_value = value;
}

TMProps const & CDidlElem::props () const
{
  return m_d->m_props;
}

TMProps& CDidlElem::props ()
{
  return m_d->m_props;
}

QString const & CDidlElem::value () const
{
  return m_d->m_value;
}

bool CDidlElem::isEmpty () const
{
  return m_d->m_props.isEmpty () && m_d->m_value.isEmpty ();
}

bool CDidlElem::isEqual (CDidlElem const & other) const
{
  bool equal = false;
  if (m_d->m_value == other.m_d->m_value)
  {
    int count = m_d->m_props.size ();
    if (count == other.m_d->m_props.size ())
    {
      for (TMProps::const_iterator it = m_d->m_props.cbegin (), end = m_d->m_props.cend (); it != end; ++it)
      {
        for (TMProps::const_iterator itOther = other.m_d->m_props.cbegin (), end = other.m_d->m_props.cend (); itOther != end; ++itOther)
        {
          if (it.value () == itOther.value ())
          {
            --count;
            break;
          }
        }
      }

      if (count == 0)
      {
        equal = true;
      }
    }
  }

  return equal;
}

QString CDidlElem::propValue (QString const & name) const
{
  return m_d->m_props.value (name);
}

CDidlItem::CDidlItem () : m_d (new SDidlItemData)
{
}

CDidlItem::CDidlItem (CDidlItem const & other) : m_d (other.m_d)
{
}

CDidlItem::~CDidlItem ()
{
}

CDidlItem& CDidlItem::operator = (CDidlItem const & other)
{
  if (this != &other)
  {
    m_d.operator = (other.m_d);
  }

  return *this;
}

int CDidlItem::sortResElems (QList<CDidlElem>& elems)
{
  if (elems.size () > 1)
  {
    QList<TQE> qes;
    qes.reserve (elems.size ());
    for (CDidlElem const & elem : elems)
    {
      TMProps const & props    = elem.props ();
      quint64         iQuality = 0;
      QString         quality  = props.value ("resolution");
      if (!quality.isEmpty ())
      {
        QStringList xy = quality.split ('x');
        if (xy.size () == 2)
        {
          iQuality         = xy[0].toUInt () * xy[1].toUInt ();
          QString protocol = props.value ("protocolInfo");
          if (protocol.indexOf ("video") != -1)
          {
            iQuality |= 0x4000000000000000;
            if (!isResConverted (protocol))
            {
              iQuality |= 0x8000000000000000;
            }
          }
        }
      }
      else
      {
        quality = props.value ("bitrate");
        if (!quality.isEmpty ())
        {
          iQuality = quality.toUInt ();
        }

        QString protocol = props.value ("protocolInfo");
        if (protocol.indexOf ("audio") != -1)
        {
          iQuality |= 0x1000000000000000;
          if (!isResConverted (protocol))
          {
            iQuality |= 0x2000000000000000;
          }
        }
      }

      TQE qe (iQuality, elem);
      qes.push_back (qe);
    }

    std::sort (qes.begin (), qes.end (), [](TQE const & a, TQE const & b) {return a.first > b.first; });
    elems.clear ();
    for (TQE const & qe : qes)
    {
      elems << qe.second;
    }
  }

  return elems.size ();
}

int CDidlItem::sortAlbumArtURIs (QList<CDidlElem>& elems)
{
  QList<TQE> qes;
  qes.reserve (elems.size ());
  for (CDidlElem const & elem : elems)
  {
    TMProps const & props    = elem.props ();
    unsigned        iQuality = 0;
    QString         quality  = props.value ("dlna:profilID");
    if (!quality.isEmpty ())
    {
      if (quality == "JPEG_SM")
      {
        iQuality = 640 * 480;
      }
      else if (quality == "JPEG_SM_ICO")
      {
        iQuality = 48 * 48;
      }
      else if (quality == "JPEG_LRG_ICO" || quality == "PNG_LRG_ICO")
      {
        iQuality = 120 * 120;
      }
      else if (quality == "JPEG_MED")
      {
        iQuality = 1024 * 768;
      }
      else if (quality == "JPEG_TN" || quality == "PNG_TH")
      {
        iQuality = 160 * 160;
      }
      else if (quality == "JPEG_LRG" || quality == "PNG_LRG")
      {
        iQuality = 4096 * 4096;
      }
      else if (quality == "GIF_LRG")
      {
        iQuality = 1600 * 1200;
      }
      else if (quality.startsWith ("JPEG_RES"))
      {
        QStringList tags = quality.split ('_');
        if (tags.size () == 3)
        {
          iQuality = tags[1].toUInt () * tags[2].toUInt ();
        }
      }
    }

    TQE qe (iQuality, elem);
    qes.push_back (qe);
  }

  std::sort (qes.begin (), qes.end (), [](TQE const & a, TQE const & b) {return a.first > b.first; });
  elems.clear ();
  for (TQE const & qe : qes)
  {
    elems << qe.second;
  }

  return qes.size ();
}

CDidlItem::EType CDidlItem::type () const
{
  EType     type = Unknown;
  CDidlElem elem = m_d->m_elems.value ("upnp:class");
  if (!elem.isEmpty ())
  {
    QString value = elem.value ();
    for (unsigned iTag = 0; iTag < sizeof (s_typeTags) / sizeof (char const *); ++iTag)
    {
      if (value == s_typeTags[iTag])
      {
        type =static_cast<EType>(iTag);
        break;
      }
    }
  }

  return type;
}

bool CDidlItem::isContainer (EType type)
{
  return type >= PlaylistItem && type < LastType;
}

bool CDidlItem::isContainer () const
{
  EType type = this->type ();
  return isContainer (type);
}

QStringList CDidlItem::stringValues (char const * tag) const
{
  QList<CDidlElem> elems = m_d->m_elems.values (tag);
  QStringList      vals;
  vals.reserve (elems.size ());
  for (CDidlElem const & elem : elems)
  {
    vals << elem.value ();
  }

  return vals;
}

QStringList CDidlItem::uris (ESortType sort) const
{
  QStringList uris;
  if (sort != SortRes)
  {
    uris = stringValues ("res");
  }
  else
  {
    QList<CDidlElem> elems = m_d->m_elems.values ("res");
    sortResElems (elems);
    uris.reserve (elems.size ());
    for (CDidlElem const & elem : elems)
    {
      uris << elem.value ();
    }
  }

  return uris;
}

QString CDidlItem::uri (int index, ESortType sort) const
{
  QStringList uris = this->uris (sort);
  QString     uri;
  if (!uris.isEmpty ())
  {
    if (index == -1)
    {
      index = uris.size () - 1;
    }

    if (index >= 0 && index < uris.size ())
    {
      uri = uris[index];
    }
  }

  return uri;
}

QStringList CDidlItem::albumArtURIs (ESortType sort) const
{
  QStringList      uris;
  QList<CDidlElem> elems = m_d->m_elems.values ("upnp:albumArtURI");
  if (elems.size () > 1 && sort == SortAlbumArt)
  {
    sortAlbumArtURIs (elems);
  }

  QList<CDidlElem> resElems = m_d->m_elems.values ("res");
  uris.reserve (elems.size () + resElems.size ());
  for (CDidlElem const & elem : elems)
  {
    uris << elem.value ();
  }

  elems.clear ();
  for (CDidlElem const & elem : resElems)
  {
    TMProps const & props    = elem.props ();
    QString         protocol = props.value ("protocolInfo");
    if (protocol.indexOf ("image") != -1)
    {
      elems << elem;
    }
  }

  if (elems.size () > 1 && sort == SortAlbumArt)
  {
    sortResElems (elems);
  }

  for (CDidlElem const & elem : elems)
  {
    uris << elem.value ();
  }

  return uris;
}

QString CDidlItem::albumArtURI (int index, ESortType sort) const
{
  QString     uri;
  QStringList uris = this->albumArtURIs (sort);
  if (!uris.isEmpty ())
  {
    if (index == -1)
    {
      index = uris.size () - 1;
    }

    if (index >= 0 && index < uris.size ())
    {
      uri = uris[index];
    }
  }

  return uri;
}

QString CDidlItem::didl (bool percentEncodeing) const
{
  QByteArray xml;
  QBuffer    buffer (&xml);
  buffer.open (QIODevice::WriteOnly);
  QXmlStreamWriter stream (&buffer);

  stream.writeStartElement ("DIDL-Lite");
  stream.writeAttribute ("xmlns", "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/");
  stream.writeAttribute ("xmlns:dc", "http://purl.org/dc/elements/1.1/");
  stream.writeAttribute ("xmlns:upnp", "urn:schemas-upnp-org:metadata-1-0/upnp/");
  stream.writeAttribute ("xmlns:dlna","urn:schemas-dlna-org:metadata-1-0/" );

  auto writeElements = [this, &stream] (QString const & tag, bool close = true)
  {
    QList<CDidlElem> elems = m_d->m_elems.values (tag);
    for (CDidlElem const & elem : elems)
    {
      stream.writeStartElement (tag);
      TMProps const & props = elem.props ();
      for (TMProps::const_iterator itProp = props.begin (); itProp != props.end (); ++itProp)
      {
        QString const & name  = itProp.key ();
        QString const & value = itProp.value ();
        stream.writeAttribute (name, value);
      }

      QString const & value = elem.value ();
      if (!value.isEmpty ())
      {
        stream.writeCharacters (value);
      }

      if (close)
      {
        stream.writeEndElement ();
      }
    }
  };

  QStringList                 keys = m_d->m_elems.uniqueKeys ();
  QStringList::const_iterator it   = std::find (keys.begin (), keys.end (), "item");
  if (it != keys.end ())
  {
    writeElements (*it, false);
    for (QString const & key : keys)
    {
      if (key != "item")
      {
        writeElements (key);
      }
    }

    stream.writeEndElement (); // End <item>
  }

  stream.writeEndElement (); // End <DIDL_Lite>
  return percentEncodeing ? toPercentEncodeing (xml) : xml;
}

QStringList CDidlItem::dump () const
{
  QStringList                      texts;
  QMapIterator<QString, CDidlElem> ite (m_d->m_elems);
  while (ite.hasNext ())
  {
    ite.next ();
    CDidlElem const & elem  = ite.value ();
    QString const &   value = elem.value ();
    TMProps const &   props = elem.props ();

    QString text = ite.key ();
    if (!props.isEmpty ())
    { // Enumerate properties.
      text += '(';
      QMapIterator<QString, QString> itp (props);
      while (itp.hasNext ())
      {
        itp.next ();
        QString const & name  = itp.key ();
        QString const & value = itp.value ();
        text                 += name + '=' + value + ',';
      }

      text.truncate (text.length () - 1);
      text += ')';
    }

    text  += '=' + value; // Add value
    texts += text;
  }

  return texts;
}

void CDidlItem::insert (QString const & name, CDidlElem const & elem)
{
  m_d->m_elems.insert (name, elem);
}

void CDidlItem::replace (QString const & name, CDidlElem const & elem)
{
  m_d->m_elems.replace (name, elem);
}

CDidlElem CDidlItem::value (QString const & name) const
{
  return m_d->m_elems.value (name);
}

QString CDidlItem::value (QString const & elemName, QString const & propName) const
{
  return m_d->m_elems.value (elemName).props ().value (propName);
}

QList<CDidlElem> CDidlItem::values (QString const & name)  const
{
  return m_d->m_elems.values (name);
}


QMultiMap<QString, CDidlElem> const & CDidlItem::elems () const
{
  return m_d->m_elems;
}

QMultiMap<QString, CDidlElem>& CDidlItem::elems ()
{
  return m_d->m_elems;
}

bool CDidlItem::isEmpty () const
{
  return m_d->m_elems.isEmpty ();
}

void CDidlItem::clear ()
{
  m_d->m_elems.clear ();
}

QString CDidlItem::toPercentEncodeing (QByteArray const & notCoded)
{
  QString encoded;
  encoded.reserve (notCoded.size () + 50);
  for (char c : notCoded)
  {
    switch (c)
    {
      case '&':
        encoded += "&amp;";
        break;

      case '\'':
        encoded += "&apos;";
        break;

      case '"':
        encoded += "&quot;";
        break;

      case '<':
        encoded += "&lt;";
        break;

      case '>':
        encoded += "&gt;";
        break;

      default:
        encoded += c;
        break;
    }
  }

  return encoded;
}

QString CDidlItem::percentDecoding (QString const & encoded)
{
  int     len = encoded.length ();
  QString decoded;
  decoded.reserve (len);
  int i = 0;
  while (i < len)
  {
    QChar c = encoded.at (i);
    if (c == '&')
    {
      if (i + 3 < len && encoded.at (i + 2) == 't' && encoded.at (i + 3) == ';')
      {
        if (encoded.at (i + 1) == 'g')
        {
          i += 3;
          c  = '>';
        }
        else if (encoded.at (i + 1) == 'l')
        {
          i += 3;
          c  = '<';
        }
      }
      else if (i + 4 < len && encoded.at (i + 1) == 'a' && encoded.at (i + 2) == 'm' && encoded.at (i + 3) == 'p' && encoded.at (i + 4) == ';')
      {
        i += 4;
        c  = '&';
      }
      else if (i + 5 < len)
      {
        if (encoded.at (i + 1) == 'a' && encoded.at (i + 2) == 'p' && encoded.at (i + 3) == 'o' && encoded.at (i + 4) == 's' && encoded.at (i + 5) == ';')
        {
          i += 5;
          c  = '\'';
        }
        else if (encoded.at (i + 1) == 'q' && encoded.at (i + 2) == 'u' && encoded.at (i + 3) == 'o' && encoded.at (i + 4) == 't' && encoded.at (i + 5) == ';')
        {
          i += 5;
          c  = '"';
        }
      }
    }

    decoded.push_back (c);
    ++i;
  }

  return decoded;
}

QString CDidlItem::id () const
{
  QString id = containerID ();
  if (id.isEmpty ())
  {
    id = itemID ();
  }

  return id;
}

QString CDidlItem::parentID () const
{
  QString id = containerParentID ();
  if (id.isEmpty ())
  {
    id = itemParentID ();
  }

  return id;
}

QString CDidlItem::itemID () const
{
  return value ("item", "id");
}

QString CDidlItem::itemParentID () const
{
  return value ("item", "parentID");
}

QString CDidlItem::containerID () const
{
  return value ("container", "id");
}

QString CDidlItem::containerParentID () const
{
  return value ("container", "parentID");
}

QString CDidlItem::title () const
{
  return m_d->m_elems.value ("dc:title").value ();
}

QString CDidlItem::artist () const
{
  return m_d->m_elems.value ("upnp:artist").value ();
}

QString CDidlItem::actor () const
{
  return m_d->m_elems.value ("upnp:actor").value ();
}

QString CDidlItem::album () const
{
  return m_d->m_elems.value ("upnp:album").value ();
}

QString CDidlItem::albumArtist () const
{
  return m_d->m_elems.value ("upnp:albumArtist").value ();
}

QString CDidlItem::creator () const
{
  return m_d->m_elems.value ("dc:creator").value ();
}

QString CDidlItem::composer () const
{
  return m_d->m_elems.value ("dc:composer").value ();
}

QString CDidlItem::genre () const
{
  return m_d->m_elems.value ("upnp:genre").value ();
}

QString CDidlItem::date () const
{
  return m_d->m_elems.value ("dc:date").value ();
}

QString CDidlItem::duration (int index) const
{
  QString          duration;
  QList<CDidlElem> elems = m_d->m_elems.values ("res");
  int              size  = elems.size ();
  if (index >= 0 && index < size)
  {
    duration = elems[index].props ().value ("duration");
  }
  else
  {
    for (CDidlElem const & elem : elems)
    {
      duration = elem.props ().value ("duration");
      if (!duration.isEmpty ())
      {
        break;
      }
    }
  }

  return duration;
}

QString CDidlItem::protocolInfo (int index) const
{
  QList<CDidlElem> elems = m_d->m_elems.values ("res");
  int              size  = elems.size ();
  return index >= 0 && index < size ? elems[index].props ().value ("protocolInfo") : QString::null;
}

QString CDidlItem::resolution (int index) const
{
  QList<CDidlElem> elems = m_d->m_elems.values ("res");
  int              size  = elems.size ();
  return index >= 0 && index < size ? elems[index].props ().value ("resolution") : QString::null;
}

unsigned CDidlItem::originalTrackNumber () const
{
  return m_d->m_elems.value ("upnp:originalTrackNumber").value ().toUInt ();
}

unsigned CDidlItem::bitrate (int index) const
{
  QList<CDidlElem> elems = m_d->m_elems.values ("res");
  int              size  = elems.size ();
  return index >= 0 && index < size ? elems[index].props ().value ("bitrate").toUInt () : 0;
}

unsigned CDidlItem::nrAudioChannels (int index) const
{
  QList<CDidlElem> elems = m_d->m_elems.values ("res");
  int              size  = elems.size ();
  return index >= 0 && index < size ? elems[index].props ().value ("nrAudioChannels").toUInt () : 0;
}

unsigned CDidlItem::size (int index) const
{
  QList<CDidlElem> elems = m_d->m_elems.values ("res");
  int              size  = elems.size ();
  return index >= 0 && index < size ? elems[index].props ().value ("size").toUInt () : 0;
}

unsigned CDidlItem::sampleFrequency (int index) const
{
  QList<CDidlElem> elems = m_d->m_elems.values ("res");
  int              size  = elems.size ();
  return index >= 0 && index < size ? elems[index].props ().value ("sampleFrequency").toUInt () : 0;
}


QString CDidlItem::description () const
{
  return m_d->m_elems.value ("upnp:description").value ();
}

QString CDidlItem::rights () const
{
  return m_d->m_elems.value ("upnp:rights").value ();
}

QString CDidlItem::language () const
{
  return m_d->m_elems.value ("upnp:language").value ();
}

QString CDidlItem::publisher () const
{
  return m_d->m_elems.value ("upnp:publisher").value ();
}

QString CDidlItem::contributor () const
{
  return m_d->m_elems.value ("upnp:contributor").value ();
}

QString CDidlItem::coverage () const
{
  return m_d->m_elems.value ("dc:coverage").value ();
}

QString CDidlItem::format () const
{
  return m_d->m_elems.value ("dc:format").value ();
}

QString CDidlItem::identifier () const
{
  return m_d->m_elems.value ("dc:identifier").value ();
}

QString CDidlItem::source () const
{
  return m_d->m_elems.value ("dc:source").value ();
}

QString CDidlItem::subject () const
{
  return m_d->m_elems.value ("dc:subject").value ();
}

CDidlItem CDidlItem::mix (CDidlItem const & item1, CDidlItem const & item2)
{
  CDidlItem item = item1;
  for (QMultiMap<QString, CDidlElem>::const_iterator it = item2.m_d->m_elems.cbegin (), end = item2.m_d->m_elems.cend (); it != end; ++it)
  {
    QString const &   name  = it.key ();
    CDidlElem const & elem2 = item2.m_d->m_elems.value (name);
    if (!item.m_d->m_elems.contains (name))
    {
      item.insert (name, elem2);
    }
    else
    {
      QList<CDidlElem> elems1 = item1.values (name);
      bool             equal  = false;
      for (QList<CDidlElem>::const_iterator it = elems1.cbegin (), end = elems1.cend (); it != end; ++it)
      {
        if (elem2.isEqual (*it))
        {
          equal = true;
          break;
        }
      }

      if (!equal)
      {
        item.insert (name, elem2);
      }
    }
  }

  return item;
}

QString CDidlItem::protocolInfoValue (QString const & protocolInfo, QString const & paramName)
{
  QString value;
  if (!protocolInfo.isEmpty ())
  {
    QStringList                 params  = protocolInfo.split (';');
    QStringList::const_iterator it      = params.cbegin (), endParam = params.cend ();
    for (; it != endParam; ++it)
    {
      QString param = (*it).trimmed ();
      if (param.startsWith (paramName))
      {
        int index = param.indexOf ('=');
        if (index != -1)
        {
          value = param.mid (index + 1).trimmed ();
          break;
        }
      }
    }
  }

  return value;
}

bool CDidlItem::isResConverted (QString const & protocolInfo)
{
  QString ciParam = protocolInfoValue (protocolInfo, "DLNA.ORG_CI");
  return ciParam == '1';
}
