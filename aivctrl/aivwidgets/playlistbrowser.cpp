#include "playlistbrowser.hpp"
#include "../upnp/avtransport.hpp"
#include "../upnp/plugin.hpp"
#include "../upnp/dump.hpp"
#include <QShortcut>
#include <QMessageBox>

USING_UPNP_NAMESPACE

CPlaylistBrowser::CPlaylistBrowser (QWidget* parent) : CContentDirectoryBrowser (parent)
{
  setDragEnabled (true);
  setDragDropMode (QAbstractItemView::InternalMove);
  connect (model (), &QAbstractItemModel::rowsMoved, this, &CPlaylistBrowser::rowsMoved);
  connect (new QShortcut (QKeySequence (Qt::Key_Delete), this), SIGNAL(activated()), this, SLOT(delKey()));
}

void CPlaylistBrowser::setAVTransportURI (CControlPoint* cp, QString const & renderer, int itemRow)
{
  CAVTransport avt (cp);
  avt.stop (renderer);
  CDevice const & device = cp->device (renderer);

  CContentDirectoryBrowserItem* cdItem   = static_cast<CContentDirectoryBrowserItem*>(item (itemRow));
  CDidlItem                     didlItem = cdItem->didlItem ();
  CDidlItem::EType              type     = didlItem.type ();
  if (!m_disableUPnPPlaylist && type != CDidlItem::AudioBroadcast && type != CDidlItem::VideoBroadcast && device.playlistStatus () == CDevice::PlaylistHandler)
  {
    QString  playlistName = CHTTPServer::formatUUID (renderer);
    int      seekTo       = 0;
    if (cp->playlistName () != playlistName)
    {
      QList<CDidlItem::TPlaylistElem> playlistElems;
      int                             cItems = count ();
      playlistElems.reserve (cItems);

      for (int iItem = 0; iItem < cItems; ++iItem)
      {
        CContentDirectoryBrowserItem* itemIItem  = static_cast<CContentDirectoryBrowserItem*>(item (iItem));
        CDidlItem                     didlItem   = itemIItem->didlItem ();
        replaceNoHttpScheme (didlItem);
        playlistElems.push_back (CDidlItem::TPlaylistElem (didlItem, 0));
        if (iItem == itemRow)
        {
          seekTo = iItem + 1;
        }
      }

      avt.setAVTransportURI (renderer, playlistName, playlistElems); // For a playlist.
      avt.waitForAVTransportURI (renderer, seekTo);
    }
    else
    {
      seekTo = itemRow + 1;
    }

    avt.seek (renderer, seekTo);
  }
  else
  {
    replaceNoHttpScheme (didlItem);
    avt.setAVTransportURI (renderer, didlItem); // For a single file.
  }

  CTransportInfo  info  = avt.getTransportInfo (renderer);
  QString const & state = info.currentTransportState ();
  if (state != "PLAYING" && state != "TRANSITIONING")
  {
    avt.play (renderer);
  }
}

void CPlaylistBrowser::setNextAVTransportURI (CControlPoint* cp, QString const & renderer)
{
  CDevice const & device = cp->device (renderer);
  if (device.hasAction (QString::null, "SetNextAVTransportURI"))
  {
    int cItems = count ();
    int iItem  = 0;
    for (; iItem < cItems; ++iItem)
    {
      QListWidgetItem* item = this->item (iItem);
      QFont            font = item->font ();
      if (font.bold ())
      {
        break;
      }
    }

    if (iItem < cItems - 1)
    {
      CContentDirectoryBrowserItem* cdItem   = static_cast<CContentDirectoryBrowserItem*>(item (iItem + 1));
      CDidlItem                     didlItem = cdItem->didlItem ();
      replaceNoHttpScheme (didlItem);
      CAVTransport (cp).setNextAVTransportURI (renderer, didlItem);
    }
  }
}

void CPlaylistBrowser::delKey ()
{
  int const maxTitles   = 10;
  int const maxTitleLen = 60;

  QList<QListWidgetItem*> items = selectedItems ();
  if (!items.isEmpty ())
  {
    clearSelection ();
    QStringList ids, titles;
    ids.reserve (items.size ());
    titles.reserve (maxTitles);
    for (QListWidgetItem* item : items)
    {
      CContentDirectoryBrowserItem* cdbItem  = static_cast<CContentDirectoryBrowserItem*>(item);
      CDidlItem const &             didlItem = cdbItem->didlItem ();
      ids.append (didlItem.itemID ());
      if (titles.size () < maxTitles)
      {
        QString title = didlItem.title ();
        if (title.length () > maxTitleLen)
        {
          title.truncate (maxTitleLen);
          title.append ("...");
        }

        titles.append (didlItem.title ());
      }
    }

    QString title = QObject::tr ("Confirm removing"), text ("<ul type=\"1\">");
    for (QString const & title : titles)
    {
      text.append ("<li>" + title + "</li>");
    }

    if (ids.size () > maxTitles)
    {
      text.append ("<li>" + QString::number (ids.size () - maxTitleLen)+ QObject::tr (" more...") + "</li>");
    }

    text.append ("</ul>");
    QMessageBox::StandardButton btn = QMessageBox::question (this, title, text);
    if (btn == QMessageBox::Yes)
    {
      for (QListWidgetItem* item : items)
      {
        delete item;
      }

      emit removeIDs (m_name, ids);
    }
  }
}

void CPlaylistBrowser::replaceNoHttpScheme (CDidlItem& didlItem)
{
  if (!m_directStreamHTTPS)
  {
    QUrl url (didlItem.uri (0));
    if (url.scheme () != "http") // Assume http protocol is handled by the renderer.
    { // Probably https
      CDidlElem const & didlElem   = didlItem.value ("aivctrl:plugin");
      QString           pluginUUID = didlElem.value ();
      if (!pluginUUID.isEmpty ())
      {
        CPlugin* plugin = m_cp->plugin (pluginUUID);
        if (plugin != nullptr)
        {
          pluginUUID         = CHTTPServer::formatUUID (pluginUUID);
          QString   uri      = m_cp->serverListenAddress () + "plugin" + '/' + pluginUUID + '/' + didlItem.uri (0);
          CDidlElem didlElem = didlItem.value ("res");
          didlElem.setValue (uri);
          didlItem.replace ("res", didlElem);
        }
      }
      else
      {
        // To do other cases.
      }
    }
  }
}
