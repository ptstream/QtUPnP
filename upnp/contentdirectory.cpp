#include "contentdirectory.hpp"
#include "actioninfo.hpp"
#include "xmlhdidllite.hpp"

USING_UPNP_NAMESPACE

CBrowseReply CContentDirectory::browse (QString const & serverUUID,
            QString const & objectID, EBrowseType type, QString const & filter,
            int startingIndex, int requestedCount, QString const & sortCriteria)
{
  Q_ASSERT (m_cp != nullptr);
  CBrowseReply reply;
  int          index = startingIndex, cReturned = 0;
  do
  {
    CBrowseReply tempReply;
    QList<CControlPoint::TArgValue> args = browseArguments (objectID, type,
                           filter, startingIndex, requestedCount, sortCriteria);
    CActionInfo actionInfo = m_cp->invokeAction (serverUUID, "Browse", args, m_browseTimeout);
    if (actionInfo.succeeded ())
    {
      tempReply.setNumberReturned (args[7].second.toUInt ());
      tempReply.setTotalMatches (args[8].second.toUInt ());
      tempReply.setUpdateID (args[9].second.toUInt ());
      cReturned = tempReply.numberReturned ();
      if (cReturned != 0)
      {
        CXmlHDidlLite h;
        h.parse (args[6].second);
        tempReply.setItems (h.items ());

        reply          += tempReply;
        index          += cReturned;
        requestedCount -= cReturned;
      }
      else
      {
        break;
      }
    }
    else
    {
      break;
    }
  }
  while (cReturned != 0 && requestedCount > 0);

  return reply;
}

CBrowseReply CContentDirectory::search (QString const & serverUUID, QString const & containerID,
             QString const & searchCriteria, QString const & filter,
             int startingIndex, int requestedCount, QString const & sortCriteria)
{
  Q_ASSERT (m_cp != nullptr);
  CBrowseReply reply;
  int          index = startingIndex, cReturned = 0;
  do
  {
    CBrowseReply                    tempReply;
    QList<CControlPoint::TArgValue> args = searchArguments (containerID, searchCriteria,
                  filter, startingIndex, requestedCount, sortCriteria);
    CActionInfo actionInfo = m_cp->invokeAction (serverUUID, "Search", args, m_browseTimeout);
    if (actionInfo.succeeded ())
    {
      tempReply.setNumberReturned (args[7].second.toUInt ());
      tempReply.setTotalMatches (args[8].second.toUInt ());
      tempReply.setUpdateID (args[9].second.toUInt ());
      cReturned = tempReply.numberReturned ();
      if (cReturned != 0)
      {
        CXmlHDidlLite h;
        h.parse (args[6].second);
        tempReply.setItems (h.items ());

        reply          += tempReply;
        index          += cReturned;
        requestedCount -= cReturned;
      }
      else
      {
        break;
      }
    }
    else
    {
      break;
    }
  }
  while (cReturned != 0 && requestedCount > 0);

  return reply;
}

QStringList CContentDirectory::getSearchCaps (QString const & serverUUID)
{
  Q_ASSERT (m_cp != nullptr);
  QStringList caps;
  QList<CControlPoint::TArgValue> args;
  args << CControlPoint::TArgValue ("SearchCaps", QString::null);
  CActionInfo actionInfo = m_cp->invokeAction (serverUUID, "GetSearchCapabilities", args);
  if (actionInfo.succeeded ())
  {
    caps = fromCVS (args.first ().second);
  }

  return caps;
}

QStringList CContentDirectory::getSortCaps (QString const & serverUUID)
{
  Q_ASSERT (m_cp != nullptr);
  QStringList                     caps;
  QList<CControlPoint::TArgValue> args;
  args << CControlPoint::TArgValue ("SortCaps", QString::null);
  CActionInfo actionInfo = m_cp->invokeAction (serverUUID, "GetSortCapabilities", args);
  if (actionInfo.succeeded ())
  {
    caps = fromCVS (args.first ().second);
  }

  return caps;
}

unsigned CContentDirectory::getSystemUpdateID (QString const & serverUUID)
{
  Q_ASSERT (m_cp != nullptr);
  unsigned                       id = 0;
  QList<CControlPoint::TArgValue> args;
  args << CControlPoint::TArgValue ("Id", QString::null);
  CActionInfo actionInfo = m_cp->invokeAction (serverUUID, "GetSystemUpdateID", args);
  if (actionInfo.succeeded ())
  {
    id = args.first ().second.toUInt ();
  }

  return id;
}

// -------------------------- Private code -----------------------------------
QList<CControlPoint::TArgValue> CContentDirectory::browseArguments (
            QString const & objectID, EBrowseType type, QString const & filter,
            int startingIndex, int requestedCount, QString const & sortCriteria)
{
  char const * browseFlag = type == BrowseDirectChildren ? "BrowseDirectChildren" : "BrowseMetadata";
  QList<CControlPoint::TArgValue> args;
  args.reserve (10);
  args << CControlPoint::TArgValue ("ObjectID", objectID);
  args << CControlPoint::TArgValue ("BrowseFlag", browseFlag);
  args << CControlPoint::TArgValue ("Filter", filter);
  args << CControlPoint::TArgValue ("StartingIndex", QString::number (startingIndex));
  args << CControlPoint::TArgValue ("RequestedCount", QString::number (requestedCount));
  args << CControlPoint::TArgValue ("SortCriteria", sortCriteria);
  args << CControlPoint::TArgValue ("Result", QString::null);
  args << CControlPoint::TArgValue ("NumberReturned", QString::null);
  args << CControlPoint::TArgValue ("TotalMatches", QString::null);
  args << CControlPoint::TArgValue ("UpdateID", QString::null);
  return  args;
}

QList<CControlPoint::TArgValue> CContentDirectory::searchArguments (QString const & containerID,
           QString const & searchCriteria, QString const & filter, int startingIndex, int requestedCount, QString const & sortCriteria)
{
  QList<CControlPoint::TArgValue> args;
  args.reserve (10);
  args << CControlPoint::TArgValue ("ContainerID", containerID);
  args << CControlPoint::TArgValue ("SearchCriteria", searchCriteria);
  args << CControlPoint::TArgValue ("Filter", filter);
  args << CControlPoint::TArgValue ("StartingIndex", QString::number (startingIndex));
  args << CControlPoint::TArgValue ("RequestedCount", QString::number (requestedCount));
  args << CControlPoint::TArgValue ("SortCriteria", sortCriteria);
  args << CControlPoint::TArgValue ("Result", QString::null);
  args << CControlPoint::TArgValue ("NumberReturned", QString::null);
  args << CControlPoint::TArgValue ("TotalMatches", QString::null);
  args << CControlPoint::TArgValue ("UpdateID", QString::null);
  return args;
}
