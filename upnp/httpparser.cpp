#include "httpparser.hpp"
#include <QFileInfo>

USING_UPNP_NAMESPACE

char const * CHTTPParser::m_playlistSuffixes[] = { "m3u", "m3u8", "wpl", "xspl", "" };

CHTTPParser::CHTTPParser (CHTTPParser const & other) : m_message (other.m_message), m_verb (other.m_verb),
            m_headerElems (other.m_headerElems), m_queryType (other.m_queryType)
{
}

CHTTPParser& CHTTPParser::operator = (CHTTPParser const & other)
{
  m_message     = other.m_message;
  m_verb        = other.m_verb;
  m_headerElems = m_headerElems;
  m_queryType   = other.m_queryType;
  return *this;
}

int CHTTPParser::headerLengthReached () const
{
  int length = m_message.indexOf ("\r\n\r\n");
  return length > 0 ? length + 4 : 0;
}

int CHTTPParser::headerContentLength () const
{
  int        len   = 0;
  QByteArray value = m_headerElems.value ("CONTENT-LENGTH");
  if (value.isEmpty ())
  {
    if (transferChunked ())
    {
      len = Chunked;
    }
    else
    {
      len = m_verb == "GET" || m_verb == "HEAD" ? 0 : Chunked;
    }
  }
  else
  {
    len = value.toInt ();
  }

  return len;
}

bool CHTTPParser::parseMessage ()
{
  bool atEnd = true;
  if (!m_message.isEmpty ())
  {
    atEnd = false;
    if (m_headerLength == 0)
    {
      // Some routers (Netgear) start sometimes the message by '\0' and ' '. Remove it.
      int k = 0, end = m_message.length ();
      while (k < end && (m_message[k] == '\0' || m_message[k] == ' '))
      {
        ++k;
      }

      if (k != 0)
      {
        m_message = m_message.mid (k);
      }

      m_headerLength = headerLengthReached ();
    }

    if (m_headerLength > 0)
    { // Header is well defined
      QByteArray        header = m_message.left (m_headerLength - 4);
      QList<QByteArray> rows;
      rows.reserve (15);
      rows = header.split ('\r');
      QByteArray name, value;
      for (QByteArray const & row : rows)
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
              m_queryType = queryType (value);
            }
          }

          m_headerElems.insert (name, value);
        }
      }

      m_contentLength = headerContentLength ();
      if (m_contentLength == 0)
      {
        atEnd = true;
      }
      else if (m_contentLength >= 0)
      { // CONTENT-LENGTH header is defined.
        int messageLength = m_message.size ();
        if (messageLength >= m_headerLength + m_contentLength)
        {
          atEnd = true;
        }
      }
      else if (m_contentLength == Chunked)
      {
        // TRANSFER-ENCODING: chunked is defined; In this case messageData.second == Chunked and represent
        // the current decoded length.
        typedef QPair<int, int> TChunk;
        QList<TChunk>           chunks;
        int                     length = m_headerLength;
        QByteArray              hexaChunkLength;
        hexaChunkLength.reserve (16);
        bool ok            = true;
        int index          = m_message.indexOf ("\r\n", length), chunkLength;
        while (index != -1 && ok)
        {
          int hexaChunkLengthSize = index - length + 2;
          hexaChunkLength         = m_message.mid (length, hexaChunkLengthSize - 2);
          chunkLength             = hexaChunkLength.toUInt (&ok, 16);
          if (ok)
          {
            int removeBegin  = length;
            int removeLength = hexaChunkLengthSize;
            if (length != m_headerLength)
            {
              removeBegin  -= 2; // Remove previous \r\n.
              removeLength += 2; // Add 2 at length to remove.
            }

            chunks.append (QPair<int, int> (removeBegin, removeLength));
            length += hexaChunkLengthSize + chunkLength + 2;
            if (chunkLength == 0)
            {
              m_message.truncate (length - 2);
              atEnd = true;
              break;
            }

            index = m_message.indexOf ("\r\n", length);
          }
        }

        for (QList<TChunk>::const_reverse_iterator it = chunks.crbegin (), end = chunks.crend (); it != end; ++it)
        {
          TChunk const & chunk = *it;
          m_message.remove (chunk.first, chunk.second);
        }
      }
    }
  }

  return atEnd;
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

CHTTPParser::EQueryType CHTTPParser::queryType (QByteArray const & query)
{
  EQueryType type = Unknown;
  if (query.contains ("/playlist/"))
  {
    type = Playlist;
  }
  else if (query.contains ("/plugin/"))
  {
    type = Plugin;
  }

  return type;
}
