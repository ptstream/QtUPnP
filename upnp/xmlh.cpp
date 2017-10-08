#include "xmlh.hpp"
#include "helper.hpp"
#include <QBuffer>
#include <QUrl>
#include <QDebug>

USING_UPNP_NAMESPACE

bool CXmlH::m_tolerantMode = false;

class CErrorHandler : public QXmlErrorHandler
{
public :
  CErrorHandler (QByteArray const & data) : QXmlErrorHandler (), m_data (data)
  {
  }

  CErrorHandler (QString const & data) : QXmlErrorHandler (), m_data (data.toUtf8 ())
  {
  }

  virtual bool error (QXmlParseException const & exception)
  {
    errorString (exception);
    return CXmlH::tolerantMode ();
  }

  virtual QString errorString () const
  {
    return "Parsing error";
  }

  virtual bool fatalError (QXmlParseException const & exception)
  {
    errorString (exception);
    return CXmlH::tolerantMode ();
  }

  virtual bool warning (QXmlParseException const & exception)
  {
    errorString (exception);
    return true;
  }

  void errorString (QXmlParseException const & exception)
  {
    // To do for generate dump file.
    qDebug () << "Line: " << exception.lineNumber ();
    qDebug () << "Column: " << exception.columnNumber ();
    qDebug () << "Message: " << exception.message ();
    qDebug () << m_data.constData ();
  }

  QByteArray m_data;
};

CXmlH::CXmlH ()
{
}

CXmlH::CXmlH (QStringList const & tags) : m_tags (tags)
{
}

CXmlH::~CXmlH ()
{
}

QString CXmlH::tag (int level) const
{
  return m_stack.size () > level ? *(m_stack.end () - level - 1) : 0;
}

bool CXmlH::parse (QByteArray response)
{
  bool success = false;
  if (!response.isEmpty ())
  {
    QByteArray data = response;
    if (!data.startsWith ("<?xml"))
    {
      data.prepend ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    }

    QBuffer          buffer (&data);
    QXmlSimpleReader reader;
    QXmlInputSource  source (&buffer);
    reader.setContentHandler (this);
    CErrorHandler errorHandler (data);
    reader.setErrorHandler (&errorHandler);
    success = reader.parse (source) | m_tolerantMode;
  }

  return success;
}

bool CXmlH::parse (QString const & response)
{
  bool success = false;
  if (!response.isEmpty ())
  {
    QString data = response;
    if (!data.startsWith ("<?xml"))
    {
      data.prepend ("<?xml version=\"1.0\" encoding=\"UTF-16\"?>\n");
    }

    QBuffer      buffer;
    char const * constChar = reinterpret_cast<char const *>(data.data ());
    buffer.setData (constChar, data.size () * 2);
    QXmlSimpleReader reader;
    QXmlInputSource  source (&buffer);
    reader.setContentHandler (this);
    CErrorHandler errorHandler (data);
    reader.setErrorHandler (&errorHandler);
    success = reader.parse (source)  | m_tolerantMode;
  }

  return success;
}

bool CXmlH::startElement (QString const &, QString const &, QString const & qName, QXmlAttributes const &)
{
  m_stack.push (qName);
  return true;
}

bool CXmlH::endElement (QString const &, QString const &, QString const &)
{
  if (!m_stack.isEmpty ())
  {
    m_stack.pop ();
  }

  return true;
}

QString CXmlH::prependSlash (QString name)
{
  if (!name.startsWith ('/'))
  {
    name.prepend ('/');
  }

  return name;
}

QString CXmlH::removeNameSpace (QString name)
{
  int index = name.indexOf (':');
  if (index != -1)
  {
    name.remove (0, index + 1);
  }

  return name;
}


QString CXmlH::ampersandHandler (QString const & data)
{
  QChar const ampersand      = QLatin1Char ('&');
  QStringList ampersandCodes = { "&amp;", "&gt;", "&lt;", "&apos;", "&quot;" };
  QString     dataCoded;
  int         length = data.length ();
  dataCoded.reserve (length + 20);

  for (int i = 0; i < length; ++i)
  {
    QChar c = data.at (i);
    if (c == ampersand)
    {
      bool ampersandIsolated = true;
      for (QString const & ampersandCode : ampersandCodes)
      {
        if (data.indexOf (ampersandCode, i) == 0)
        {
          ampersandIsolated = false;
        }
      }

      if (ampersandIsolated)
      {
        dataCoded.append (ampersandCodes.first ());
      }
      else
      {
        dataCoded.append (c);
      }
    }
    else
    {
      dataCoded.append (c);
    }
  }

  return dataCoded;
}
