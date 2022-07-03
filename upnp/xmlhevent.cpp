#include "xmlhevent.hpp"

USING_UPNP_NAMESPACE

CXmlHEvent::CXmlHEvent (TMEventVars& vars) : m_vars (vars)
{
}

bool CXmlHEvent::startElement (QString const & namespaceURI, QString const & localName, QString const & qName, QXmlAttributes const & atts)
{
  CXmlH::startElement (namespaceURI, localName, qName, atts);
  QString tagParent = this->tagParent ();
  if (!m_checkProperty || tagParent == "e:property")
  {
    TEventValue val;
    for (int iAtt = 0, cAtts = atts.count (); iAtt < cAtts; ++iAtt)
    {
      QString name  = atts.qName (iAtt);
      QString value = atts.value (iAtt);
      if (name == "val")
      {
        val.first = value;
      }
      else
      {
        val.second.push_back (TEventCst (name, value));
      }
    }

    if (qName == "Event")
    {
      val.first = namespaceURI;
    }

    QString name = removeNameSpace (qName);
    m_vars.insertMulti (name, val);
  }

  return true;
}

bool CXmlHEvent::characters (QString const & name)
{
  QString tag = m_stack.top ();
  if (tagParent () == "e:property")
  {
    TEventValue value;
    value.first                   = name;
    m_vars.insert(removeNameSpace (tag), value);
  }

  return true;
}
