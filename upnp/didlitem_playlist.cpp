#include "didlitem.hpp"
#include "helper.hpp"

USING_UPNP_NAMESPACE

QByteArray CDidlItem::m3u (QList<TPlaylistElem> const & playlistElems)
{
  QByteArray content ("#EXTM3U\r\n");
  for (TPlaylistElem const & playlistElem : playlistElems)
  {
    QList<CDidlElem> resElems = playlistElem.first.values ("res");
    if (!resElems.isEmpty ())
    {
      CDidlItem::sortResElems (resElems);
      int              index    = playlistElem.second;
      if (index < 0 || index >= resElems.size ())
      {
        index = 0;
      }

      CDidlElem const & resElem  = resElems[index];
      content                   += resElem.value ().toLatin1 ();
      QString           duration = resElem.props ().value ("duration");
      unsigned          s        = timeToMS (duration) / 1000;
      QString const &   title    = playlistElem.first.value ("dc:title").value ();
      content                   += "\r\n#EXTINF:" + QByteArray::number (s) + ',' + title.toLatin1 () + "\r\n";
    }
  }

  content += "\r\n";
  return content;
}

QByteArray CDidlItem::m3u8 (QList<TPlaylistElem> const & playlistElems)
{
  QByteArray content ("#EXTM3U\r\n");
  for (TPlaylistElem const & playlistElem : playlistElems)
  {
    QList<CDidlElem> const & resElems = playlistElem.first.values ("res");
    if (!resElems.isEmpty ())
    {
      int              index    = playlistElem.second;
      if (index < 0 || index >= resElems.size ())
      {
        index = 0;
      }

      CDidlElem const & resElem  = resElems[index];
      content                   += resElem.value ().toUtf8 ();
      QString           duration = resElem.props ().value ("duration");
      unsigned          s        = timeToMS (duration) / 1000;
      QString const &   title    = playlistElem.first.value ("dc:title").value ();
      content                   += "\r\n#EXTINF:" + QByteArray::number (s) + ',' + title.toUtf8 () + "\r\n";
    }
  }

  content += "\r\n";
  return content;
}

QByteArray CDidlItem::buildsPlaylist (QList<TPlaylistElem> const & elems, EPlaylistFormat format)
{
  QByteArray content;
  switch (format)
  {
    case M3u:
      content = m3u (elems);
      break;

   case M3u8:
      content = m3u8 (elems);
      break;

    default:
      break;
  }

  return content;
}
