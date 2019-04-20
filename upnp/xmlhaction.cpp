#include "xmlhaction.hpp"

USING_UPNP_NAMESPACE

CXmlHAction::CXmlHAction (QString const & actionName, QMap<QString, QString>& vars) : m_actionName (actionName), m_vars (vars)
{
}

bool CXmlHAction::characters (QString const & name)
{
  QString const & tag  = m_stack.top ();
  if (tag == "u:errorCode")
  {
    m_errorCode = name.toInt ();

  }
  else if (tag == "u:errorDescription")
  {
    m_errorDesc = name;
  }
  else
  {
    QString pTag = tagParent ();
    if (pTag.endsWith (m_actionName + "Response"))
    {
      m_vars.insert (tag, name);
    }
  }

  return true;
}

