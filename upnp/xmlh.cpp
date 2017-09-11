#include "xmlh.hpp"
#include <QBuffer>

USING_UPNP_NAMESPACE

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
    QBuffer          buffer (&response);
    QXmlSimpleReader reader;
    QXmlInputSource  source (&buffer);
    reader.setContentHandler (this);
    reader.setErrorHandler (this);
    success = reader.parse (source);
  }

  return success;
}

bool CXmlH::parse (QString response)
{
  bool success = false;
  if (!response.isEmpty ())
  {
    QBuffer buffer;
    buffer.setData (reinterpret_cast<char *>(response.data ()), response.size () * 2);
    QXmlSimpleReader reader;
    QXmlInputSource  source (&buffer);
    reader.setContentHandler (this);
    reader.setErrorHandler (this);
    success = reader.parse (source);
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
