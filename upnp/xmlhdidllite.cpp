#include "xmlhdidllite.hpp"

USING_UPNP_NAMESPACE

CXmlHDidlLite::CXmlHDidlLite ()
{
}

bool CXmlHDidlLite::startElement (QString const & namespaceURI, QString const & localName, QString const & qName, QXmlAttributes const & atts)
{
  if (qName != "DIDL-Lite")
  {
    CXmlH::startElement (namespaceURI, localName, qName, atts);

    TMProps props;
    for (int iAtt = 0; iAtt < atts.count (); ++iAtt)
    {
      QString const & name = atts.qName (iAtt);
      QString const & value = atts.value (iAtt);
      props[name] = value;
    }

    CDidlElem elem;
    elem.setProps (props);
    if (qName == "item" || qName == "container")
    {
      CDidlItem item;
      item.insert (qName, elem);
      m_items.push_back (item);
    }
    else if (!m_items.isEmpty ())
    {
      m_items.last ().insert (qName, elem);
    }
  }

  return true;
}

bool CXmlHDidlLite::characters (QString const & name)
{
  if (!m_stack.isEmpty () && !m_items.isEmpty () && !name.trimmed ().isEmpty ())
  {
    QString const & tag  = m_stack.top ();
    CDidlItem&      item = m_items.last ();
    CDidlElem       elem = item.value (tag);
    elem.setValue (name);
    item.replace (tag, elem);
  }

  return true;
}

CDidlItem CXmlHDidlLite::firstItem (QByteArray data)
{
  CDidlItem item;
  bool      success = parse (data);
  if (success && !m_items.isEmpty ())
  {
    item = m_items.first ();
  }

  return item;
}

CDidlItem CXmlHDidlLite::firstItem (QString const & didlLite)
{
  CDidlItem item;
  QString   data    = ampersandHandler (didlLite);
  bool      success = parse (data);
  if (success && !m_items.isEmpty ())
  {
    item = m_items.first ();
  }

  return item;
}
