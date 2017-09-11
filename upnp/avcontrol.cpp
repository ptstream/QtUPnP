#include "avcontrol.hpp"

USING_UPNP_NAMESPACE

CAVControl::CAVControl ()
{
}

CAVControl::CAVControl (CControlPoint* cp) : m_cm (cp), m_cd (cp), m_tr (cp), m_rc (cp)
{
}

void CAVControl::setControlPoint (CControlPoint* cp)
{
  m_cm.setControlPoint (cp);
  m_cd.setControlPoint (cp);
  m_tr.setControlPoint (cp);
  m_rc.setControlPoint (cp);
}

CControlPoint* CAVControl::controlPoint ()
{
  return m_cm.controlPoint ();
}
