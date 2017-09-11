#include "xmlhservice.hpp"
#include "service.hpp"

USING_UPNP_NAMESPACE

CXmlHService::CXmlHService (CService& service) : m_service (service)
{
}

bool CXmlHService::startElement (QString const & namespaceURI, QString const & localName, QString const & qName, QXmlAttributes const & atts)
{
  CXmlH::startElement (namespaceURI, localName, qName, atts);
  if (qName == "stateVariable")
  {
    m_var.first.clear ();
    m_var.second = CStateVariable ();
    if (atts.count () > 0)
    {
      QString const & value = atts.value (0);
      if (!value.isEmpty ())
      {
        bool envented = value[0] == 'y' || value[0] == 'Y' || value[0] == '1' || value[0] == 't'  || value[0] == 'T';
        m_var.second.setEvented (envented);
      }
    }
  }
  else if (qName == "action")
  {
    m_action.first.clear ();
    m_action.second = CAction ();
  }
  else if (qName == "argument")
  {
    m_arg.first.clear ();
    m_arg.second = CArgument ();
  }

  return true;
}

bool CXmlHService::characters (QString const & name)
{
  QString const & tag = m_stack.top ();
  if (tag == "major")
  {
    m_service.setMajorVersion (name.toUInt ());
  }
  else if (tag == "minor")
  {
    m_service.setMinorVersion (name.toUInt ());
  }
  else if (tag == "name")
  {
    QString parent = this->tagParent ();
    if (parent == "stateVariable")
    {
      m_var.first = name;
    }
    else if (parent == "action")
    {
      m_action.first = name;
    }
    else if (parent == "argument")
    {
      m_arg.first = name;
    }
  }
  else if (tag == "dataType")
  {
    m_var.second.setType (name);
  }
  else if (tag == "minimum")
  {
    m_var.second.setMinimum (name.toDouble ());
  }
  else if (tag == "maximum")
  {
    m_var.second.setMaximum (name.toDouble ());
  }
  else if (tag == "step")
  {
    m_var.second.setStep (name.toDouble ());
  }
  else if (tag == "allowedValue")
  {
    m_var.second.addAllowedValue (name);
  }
  else if (tag == "direction")
  {
    CArgument::EDir dir = CArgument::Unknown;
    if (!name.isEmpty ())
    {
      dir = name[0] == 'i' || name[0] == 'I' ? CArgument::In : CArgument::Out;
    }

    m_arg.second.setDir (dir);
  }
  else if (tag == "relatedStateVariable")
  {
    m_arg.second.setRelatedStateVariable (name);
  }

  return true;
}

bool CXmlHService::endElement (QString const & namespaceURI, QString const & localName, QString const & qName)
{
  CXmlH::endElement (namespaceURI, localName, qName);
  if (qName == "action")
  {
    m_service.actions ().insert (m_action.first, m_action.second);
  }
  else if (qName == "stateVariable")
  {
    m_service.stateVariables ().insert (m_var.first, m_var.second);
  }
  else if (qName == "argument")
  {
    m_action.second.arguments ().insert (m_arg.first, m_arg.second);
  }

  return true;
}
