#include "playlistbrowser.hpp"
#include "../upnp/avtransport.hpp"
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
  CDidlItem const &             didlItem = cdItem->didlItem ();
  CDidlItem::EType              type     = didlItem.type ();
  if (type != CDidlItem::AudioBroadcast && type != CDidlItem::VideoBroadcast && device.playlistStatus () == CDevice::PlaylistHandler)
  {
    QString  playlistName = CHTTPServer::playlistBaseName (renderer);
    int      seekTo       = 0;
    if (cp->playlistName () != playlistName)
    {
      QList<CDidlItem::TPlaylistElem> playlistElems;
      int                             cItems = count ();
      playlistElems.reserve (cItems);

      for (int iItem = 0; iItem < cItems; ++iItem)
      {
        CContentDirectoryBrowserItem* itemIItem = static_cast<CContentDirectoryBrowserItem*>(item (iItem));
        CDidlItem const &             didlItem  = itemIItem->didlItem ();
        playlistElems.push_back (CDidlItem::TPlaylistElem (didlItem, 0));
        if (iItem == itemRow)
        {
          seekTo = iItem + 1;
        }
      }

      avt.setAVTransportURI (renderer, playlistName, playlistElems);
      avt.waitForAVTransportURI (renderer, seekTo);
    }
    else
    {
      seekTo = itemRow;
    }

    avt.seek (renderer, seekTo);
  }
  else
  {
    avt.setAVTransportURI (renderer, didlItem);
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
      CDidlItem const &             didlItem = cdItem->didlItem ();
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
      emit removeIDs (m_name, ids);
      for (QListWidgetItem* item : items)
      {
        delete item;
      }
    }
  }
}
