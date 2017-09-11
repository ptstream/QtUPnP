#include "control.hpp"
#include "actioninfo.hpp"

USING_UPNP_NAMESPACE

QStringList CControl::fromCVS (QString entryString)
{
  QStringList list;
  if (!entryString.isEmpty ())
  {
    list = entryString.remove (' ').split (',');
  }

  return list;
}

bool CControl::toBool (QString const & entryString)
{
  bool value = false;
  if (!entryString.isEmpty ())
  {
    value = entryString[0] != '0' && entryString[0] != 'f' && entryString[0] != 'F';
  }

  return value;
}




