#include "serverscanner.hpp"
#include "../upnp/contentdirectory.hpp"
#include <QProgressDialog>

USING_UPNP_NAMESPACE

CServerScanner::CServerScanner (CControlPoint* cp, QtUPnP::CDidlItem::EType type, QProgressDialog* progress) :
                                m_cp (cp), m_type (type), m_progress (progress)
{
  switch (type)
  {
    case CDidlItem::AudioItem :
    case CDidlItem::MusicTrack :
    case CDidlItem::AudioBroadcast :
    case CDidlItem::AudioBook :
    case CDidlItem::AudioProgram :
      m_containerTypes << CDidlItem::PlaylistItem
                       << CDidlItem::Container
                       << CDidlItem::PlaylistContainer
                       << CDidlItem::MusicAlbum
                       << CDidlItem::StorageSystem
                       << CDidlItem::StorageVolume
                       << CDidlItem::StorageFolder
                       << CDidlItem::MusicArtist
                       << CDidlItem::MusicGenre
                       << CDidlItem::Genre
                       << CDidlItem::AudioChannelGroup;
      break;

    case CDidlItem::ImageItem :
    case CDidlItem::Photo :
      m_containerTypes << CDidlItem::PlaylistItem
                       << CDidlItem::PhotoAlbum
                       << CDidlItem::Container
                       << CDidlItem::PlaylistContainer
                       << CDidlItem::StorageSystem
                       << CDidlItem::StorageVolume
                       << CDidlItem::StorageFolder;
      break;

    case CDidlItem::VideoItem :
    case CDidlItem::Movie :
    case CDidlItem::VideoBroadcast :
    case CDidlItem::MusicVideoClip :
    case CDidlItem::VideoProgram :
      m_containerTypes << CDidlItem::PlaylistItem
                       << CDidlItem::Container
                       << CDidlItem::PlaylistContainer
                       << CDidlItem::StorageSystem
                       << CDidlItem::StorageVolume
                       << CDidlItem::StorageFolder
                       << CDidlItem::MovieGenre
                       << CDidlItem::Genre
                       << CDidlItem::VideoChannelGroup;                            ;
      break;

    default :
      break;
  }
}

CDidlItem CServerScanner::scan (QString const & uuid, QString const & title)
{
  if (m_progress != nullptr)
  {
    m_currentProgressValue = m_progress->value ();
  }

  SServerData& server = m_servers[uuid];
  CDidlItem    item   = server.m_titles.value (title);
  if (item.isEmpty ())
  {
    if (server.m_containers.isEmpty ())
    {
      server.m_containers.enqueue (SContainer ("0", QObject::tr ("Root")));
    }

    item = browse (uuid, title);
  }

  return item;
}

void CServerScanner::setProgressLabel (QString const & title)
{
  if (m_progress != nullptr)
  {
    m_progressValue += m_progressInc;
    if (m_progressValue == m_progress->maximum ())
    {
      m_progressInc    = -m_progressInc,
      m_progressValue += m_progressInc;
    }
    else if (m_progressValue == m_currentProgressValue)
    {
      m_progressInc    = -m_progressInc,
      m_progressValue += m_progressInc;
    }

    m_progress->setValue (m_progressValue);
    QString text = title;
    if (text.length () > m_progressTitleLength)
    {
      text.truncate (m_progressTitleLength);
      text += "...";
    }

    m_progress->setLabelText (QObject::tr ("Scan: ") + text);
  }
}

CDidlItem CServerScanner::browse (QString const & uuid, QString const & title)
{
  CContentDirectory cd (m_cp);
  CDidlItem         newDidlItem;
  SServerData&      server = m_servers[uuid];
  while (!server.m_containers.isEmpty ())
  {
    SContainer container = server.m_containers.dequeue ();
    setProgressLabel (container.m_title);
    CBrowseReply replies = cd.browse (uuid, container.m_id);
    for (CDidlItem const & item : replies.items ())
    {
      QString itemTitle = item.title ();
      if (!item.isContainer ())
      {
        if (item.type () == m_type)
        {
          if (!server.m_titles.contains (itemTitle))
          {
            server.m_titles.insert (itemTitle, item);
          }

          if (itemTitle == title)
          {
            newDidlItem = item;
          }
        }
        else
        {
          break;
        }
      }
      else
      {
        TContainers::const_iterator end = server.m_containers.cend ();
        QString                     id  = item.containerID ();
        if (m_containerTypes.contains (item.type ()) && std::find (server.m_containers.cbegin (), end, id) == end)
        {
          server.m_containers.enqueue (SContainer (id, item.title ()));
        }
      }
    }

    if (!newDidlItem.isEmpty () || m_progress->wasCanceled ())
    {
      break;
    }
  }

  return newDidlItem;
}

