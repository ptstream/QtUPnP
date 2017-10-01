#include "httpparser.hpp"
#include <QFileInfo>

USING_UPNP_NAMESPACE

char const * CHTTPParser::m_playlistSuffixes[] = { "m3u", "m3u8", "wpl", "xspl", "" };

int CHTTPParser::headerLength () const
{
  int length = m_message.indexOf ("\r\n\r\n");
  return length > 0 ? length + 4 : 0;
}

int CHTTPParser::contentLength () const
{
  int        len   = 0;
  QByteArray value = m_headerElems.value ("CONTENT-LENGTH");
  if (value.isEmpty () && transferChunked ())
  {
    len = -1;
  }
  else
  {
    len = value.toInt ();
  }

  return len;
}

void CHTTPParser::parseMessage ()
{
  if (!m_message.isEmpty ())
  {
    // Some router (Netgear) start sometimes the message by '\0' and blank. Remove it.
    int k = 0;
    for (int end = m_message.length (); k < end && (m_message[k] == '\0' || m_message[k] == ' '); ++k);
    if (k != 0)
    {
      m_message = m_message.mid (k);
    }

    int headerLength = this->headerLength ();
    if (headerLength > 0)
    {
      QByteArray        header = m_message.left (headerLength - 4);
      QList<QByteArray> rows;
      rows.reserve (15);
      rows = header.split ('\r');

      QByteArray name, value;
      for (QByteArray const row : rows)
      {
        int index = row.indexOf (':');
        if (index == -1 && m_verb.isEmpty ())
        {
          index = row.indexOf (' ');
        }

        if (index != -1)
        {
          name  = row.left (index).trimmed ().toUpper ();
          value = row.mid (index + 1).trimmed ();
          if (m_verb.isEmpty ())
          {
            m_verb = name;
            if (name == "GET" || name == "HEAD")
            {
              value.truncate (value.length () - 9);
              m_playlistPath = isPlaylistPath (value);
            }
          }

          m_headerElems.insert (name, value);
        }
      }
    }
  }
}

bool CHTTPParser::transferChunked () const
{
  bool chunked = value ("TRANSFER-ENCODING") == "chunked";
  if (!chunked)
  {
    chunked = value ("TRANSFER-ENCODING").toLower () == "chunked";
  }

  return chunked;
}

bool CHTTPParser::isPlaylistPath (QString const & path)
{
  bool      playlist = false;
  QFileInfo fi (path);
  QString   pathSuffix = fi.suffix ();
  for (int k = 0; m_playlistSuffixes[k][0] != '\0'; ++k)
  {
    if (pathSuffix == m_playlistSuffixes[k])
    {
      playlist = true;
      break;
    }
  }

  return playlist;
}
