#include "xmlh.hpp"
#include "helper.hpp"
#include "dump.hpp"
#include <QBuffer>
#include <QUrl>
#include <QDateTime>
#include <QDebug>

USING_UPNP_NAMESPACE

bool CXmlH::m_tolerantMode = true;
QString CXmlH::m_dumpErrorFileName;

class CErrorHandler : public QXmlErrorHandler
{
public :
  CErrorHandler (QByteArray const & data, QString const & fileName);
  CErrorHandler (QString const & data, QString const & fileName);

  virtual bool error (QXmlParseException const & exception);
  virtual QString errorString () const;
  virtual bool fatalError (QXmlParseException const & exception);
  virtual bool warning (QXmlParseException const & exception);
  void errorString (QXmlParseException const & exception);

private :
  QByteArray m_data;
  QString m_fileName;
};

CErrorHandler::CErrorHandler (QByteArray const & data, QString const & fileName) : QXmlErrorHandler (),
  m_data (data), m_fileName (fileName)
{
}

CErrorHandler::CErrorHandler (QString const & data, QString const & fileName) : QXmlErrorHandler (),
  m_data (data.toUtf8 ()), m_fileName (fileName)
{
}

bool CErrorHandler::error (QXmlParseException const & exception)
{
  errorString (exception);
  return CXmlH::tolerantMode ();
}

QString CErrorHandler::errorString () const
{
  return "UPnP XML error during parsing";
}

bool CErrorHandler::fatalError (QXmlParseException const & exception)
{
  errorString (exception);
  return CXmlH::tolerantMode ();
}

bool CErrorHandler::warning (QXmlParseException const & exception)
{
  errorString (exception);
  return true;
}

void CErrorHandler::errorString (QXmlParseException const & exception)
{
  QString text = QString ("XML error; line: %2; column: %3; message: %4\n%5\n")
                 .arg (exception.lineNumber ()).arg (exception.columnNumber ())
                 .arg (exception.message ()).arg (m_data.constData ());
  if (!m_fileName.isEmpty())
  {
    QFile file (m_fileName);
    if (file.open (QIODevice::Append | QIODevice::Text))
    {
      file.write (text.toUtf8 ());
      file.close ();
    }
  }
  else
  {
    qDebug () << text;
    CDump::dump (text + '\n');
  }
}

CXmlH::CXmlH ()
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
    CErrorHandler errorHandler (data, m_dumpErrorFileName);
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
    CErrorHandler errorHandler (data, m_dumpErrorFileName);
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
  QChar const      ampersand        = QLatin1Char ('&');
  QString          ampersandCodes[] = { "&amp;", "&gt;", "&lt;", "&apos;", "&quot;" };
  QChar            replacements[]   = { '&',     '>',    '<',     '\'',    '"' };
  QString          dataCoded;
  int              length = data.length ();
  int              cAmpersandCodes = sizeof (ampersandCodes) / sizeof (QString);

  dataCoded.reserve (length + 50);

  for (int i = 0; i < length; ++i)
  {
    QChar c = data.at (i);
    if (c == ampersand)
    {
      int iAmpersandCode = 0;
      for (;iAmpersandCode < cAmpersandCodes; ++iAmpersandCode)
      {
        if (data.indexOf (ampersandCodes[iAmpersandCode], i) == i)
        {
          break;
        }
      }

      if (iAmpersandCode < cAmpersandCodes)
      {
        dataCoded.append (replacements[iAmpersandCode]);
        i += ampersandCodes[iAmpersandCode].length () - 1;
      }
      else
      {
        dataCoded.append ("%26");
      }
    }
    else
    {
      dataCoded.append (c);
    }
  }

  return dataCoded;
}
