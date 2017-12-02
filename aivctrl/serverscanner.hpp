#ifndef SERVERSCANNER_HPP
#define SERVERSCANNER_HPP

#include "../upnp/didlitem.hpp"
#include <QHash>
#include <QQueue>
#include <QSet>

namespace QtUPnP {
  class CControlPoint;
}

class QProgressDialog;

/*! This class provides functionalities to browse a server. It is used to checked playlist items.
 * The class scan a server to retreave an item from a title.
 * A set of optimizations is use to speedup the search.
 */
class CServerScanner
{
public:
  /*! Items already found. */
  typedef QHash<QString, QtUPnP::CDidlItem> TDidlItem;

  /*! Container already scanned. */
  struct SContainer
  {
    SContainer () {}
    SContainer (QString const & id, QString const & title) : m_id (id), m_title (title) {}
    SContainer (SContainer const & other) : m_id (other.m_id), m_title (other.m_title) {}

    SContainer operator = (SContainer const & other)
    {
      m_id    = other.m_id;
      m_title = other.m_title;
      return *this;
    }

    bool operator == (SContainer const & other) const
    {
      return m_id == other.m_id;
    }

    bool operator == (QString const & id) const
    {
      return m_id == id;
    }

    QString m_id, m_title;
  };

  /*! Current queue containers already scanned. */
  typedef QQueue<SContainer> TContainers;

  /*! Actual server scanned. */
  struct SServerData
  {
    SServerData () {}
    SServerData (SServerData const & other) : m_titles (other.m_titles), m_containers (other.m_containers) {}

    SServerData& operator = (SServerData const & other)
    {
      m_titles     = other.m_titles;
      m_containers = other.m_containers;
      return *this;
    }

    TDidlItem m_titles;
    TContainers m_containers;
  };

  /*! Constructor. */
  CServerScanner (QtUPnP::CControlPoint* cp, QtUPnP::CDidlItem::EType type, QProgressDialog* progress);

  /*! Returns the contril point. */
  QtUPnP::CControlPoint* controlPoint () { return m_cp; }

  /*! Scans the server. */
  QtUPnP::CDidlItem scan (QString const & uuid, QString const & title);

  /*! Invokes browse action. */
  QtUPnP::CDidlItem browse (QString const & uuid, QString const & title);

  void setProgressLabel (QString const & title);

private :
  QtUPnP::CControlPoint* m_cp = nullptr;
  QMap<QString, SServerData> m_servers;
  QtUPnP::CDidlItem::EType m_type = QtUPnP::CDidlItem::Unknown;
  QSet<QtUPnP::CDidlItem::EType> m_containerTypes;
  QProgressDialog* m_progress = nullptr;
  int m_currentProgressValue = 0;
  int m_progressValue = 0;
  int const m_progressTitleLength = 30;
  int m_progressInc = 1;
};

#endif // SERVERSCANNER_HPP
