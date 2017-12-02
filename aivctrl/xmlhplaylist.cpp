#include "xmlhplaylist.hpp"
#include "playlist.hpp"
#include "../upnp/xmlhdidllite.hpp"
#include <QTime>

USING_UPNP_NAMESPACE

CXmlHPlaylist::CXmlHPlaylist (CPlaylist& playlist) : CXmlH (), m_playlist (playlist)
{
}

bool CXmlHPlaylist::startElement (QString const & namespaceURI, QString const & localName, QString const & qName, QXmlAttributes const & atts)
{
  CXmlH::startElement (namespaceURI, localName, qName, atts);
  if (localName == "track")
  {
    CDidlItem item;
    item.insert ("item", CDidlElem ());
    m_playlist.items () << item;
  }

  return true;
}

bool CXmlHPlaylist::characters (QString const & name)
{
  if (!m_stack.isEmpty () && !name.trimmed ().isEmpty ())
  {
    QString const & tag = m_stack.top ();
    if (tag == "title")
    {
      QString tagParent = this->tagParent ();
      if (tagParent == "playlist")
      {
        m_name = name;
      }
      else
      {
        CDidlElem elem;
        elem.setValue (name);
        m_playlist.items ().last ().insert ("dc:title", elem);
      }
    }
    else if (tag == "location")
    {
      CDidlItem& item = m_playlist.items ().last ();
      if (!item.elems ().contains ("res"))
      {
        CDidlElem elem;
        elem.setValue (name);
        item.insert ("res", elem);
      }
      else
      {
        CDidlElem elem = m_playlist.items ().last ().elems ().value ("res");
        elem.setValue (name);
        item.replace ("res", elem);
      }
    }
    else if (tag == "album")
    {
      CDidlElem elem;
      elem.setValue (name);
      m_playlist.items ().last ().insert ("upnp:album", elem);
    }
    else if (tag == "duration")
    {
      CDidlItem& item     = m_playlist.items ().last ();
      QString    duration = QTime (0, 0).addMSecs (name.toInt ()).toString ("hh:mm:ss.zzz");
      if (!item.elems ().contains ("res"))
      {
        CDidlElem elem;
        elem.addProp ("duration", duration);
        item.insert ("res", elem);
      }
      else
      {
        CDidlElem elem = m_playlist.items ().last ().elems ().value ("res");
        elem.addProp ("duration", duration);
        item.replace ("res", elem);
      }
    }
    else if (tag == "image")
    {
      CDidlElem elem;
      elem.setValue (name);
      m_playlist.items ().last ().insert ("upnp:albumArtURI", elem);
    }
    else if (tag == "aivctrl:nrTracks")
    {
      m_playlist.items ().reserve (name.toInt ());
    }
    else if (tag == "aivctrl:playlistType")
    {
      m_playlist.setType (static_cast<CPlaylist::EType>(name.toInt ()));
    }
    else if (tag == "aivctrl:upnpID")
    {
      CDidlElem elem = m_playlist.items ().last ().elems ().value ("item");
      elem.addProp ("item", name);
      m_playlist.items ().last ().replace ("item", elem);
    }
    else if (tag == "meta")
    {
      CDidlItem& item = m_playlist.items ().last ();
      CXmlHDidlLite didLite;
      item = CDidlItem::mix (didLite.firstItem (name), item);
    }
  }

  return true;
}
