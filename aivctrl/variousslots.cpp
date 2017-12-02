#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "aivwidgets/networkprogress.hpp"
#include "aivwidgets/widgethelper.hpp"
#include "../upnp/renderingcontrol.hpp"

USING_UPNP_NAMESPACE

void CMainWindow::changeFolder (int index)
{
  searchAction (false);
  if (index == 0)
  {
    on_m_home_clicked ();
  }
  else if (ui->m_folders->erase (index))
  {
    CFolderItem const & item = ui->m_folders->top ();
    updateContentDirectory (item, false);
  }
}

void CMainWindow::on_m_stackedWidget_currentChanged (int index)
{
  searchAction (false);
  QToolButton::ToolButtonPopupMode popupMode = QToolButton::DelayedPopup;
  switch (index)
  {
    case Home :
      CContentDirectoryBrowser::stopIconUpdateTimer ();
      ui->m_previousStackedWidgetIndex->setEnabled (false);
      ui->m_folders->clearStack ();
      ui->m_playlistContent->clear ();
      ui->m_provider->clear ();
      updatePlaylistItemCount ();
      popupMode = QToolButton::InstantPopup;
      break;

    case ContentDirectory :
      ui->m_contentDirectory->startIconUpdateTimer ();
      ui->m_previousStackedWidgetIndex->setEnabled (true);
      ui->m_folders->updateText ();
      ui->m_playlistContent->clear ();
      ui->m_contentDirectory->setFocus ();
      break;

    case Queue :
      ui->m_queue->startIconUpdateTimer ();
      ui->m_folders->setText (tr ("Queue"));
      ui->m_currentQueuePlaying->setIcon (::resIcon ("queue"));
      ui->m_previousStackedWidgetIndex->setEnabled (true);
      ui->m_playlistContent->clear ();
      ui->m_queue->setFocus ();
      break;

    case Playing :
      ui->m_currentQueuePlaying->setIcon (::resIcon ("current_playing"));
      ui->m_previousStackedWidgetIndex->setEnabled (true);
      ui->m_playlistContent->clear ();
      break;

    case Playlist :
      ui->m_playlistContent->startIconUpdateTimer ();
      ui->m_previousStackedWidgetIndex->setEnabled (true);
      ui->m_playlistContent->setFocus ();
      break;
  }

  bool hidden = index == Playing;
  ui->m_footer->setHidden (hidden);
  ui->m_myDevice->clearSelection ();
  ui->m_home->setPopupMode (popupMode);
}

void CMainWindow::updatePosition ()
{
  if (!ui->m_position->mousePressed ())
  {
    int const maxAbsTime  = ui->m_position->maxAbsTime ();
    int const maxAbsCount = 2000000000; // Probably to big :).

    // Get position from GetPositionInfo action.
    ui->m_position->updatePosition (m_cp, m_renderer);
    CPositionInfo const & positionInfo = ui->m_position->positionInfo ();

    QString const & trackURI = positionInfo.trackURI ();
    if (!trackURI.isEmpty ())
    {
      int relTimeMS = ui->m_position->value ();
      int absTimeMS = ui->m_position->maximum ();

      // Update absTime and relTime widgets.
      bool enable    = absTimeMS >= 0 && absTimeMS <= maxAbsTime;
      ui->m_relTime->setEnabled (enable);
      ui->m_absTime->setEnabled (enable);
      if (!enable)
      {
        relTimeMS = absTimeMS = 0;
      }

      if (ui->m_absTime->isChecked ())
      {
        absTimeMS -= relTimeMS;
      }

      QTime         absTime   = QTime (0, 0).addMSecs (absTimeMS);
      QString const formatOut = absTime.hour () == 0 ? "mm:ss" : "hh:mm:ss";
      ui->m_absTime->setText (absTime.toString (formatOut));

      QTime relTime = QTime (0, 0).addMSecs (relTimeMS);
      ui->m_relTime->setText (relTime.toString (formatOut));

      // Update rank widget.
      QString numbers;
      int     absCount = positionInfo.absCount ();
      int     relCount = 0;
      if (absCount > 0 && absCount < maxAbsCount)
      {
        relCount = positionInfo.relCount ();
      }

      // If relCount not defined, get value from queue list widget.
      if (relCount <= 0 || relCount > maxAbsCount)
      {
        relCount = ui->m_queue->boldIndex () + 1;
        absCount = ui->m_queue->count ();
      }

      numbers = QString ("%1/%2").arg (relCount).arg (absCount);
      ui->m_rank->setText (numbers);

      // Set bold the current playing item.
      setItemBold (trackURI);
    }
  }
}

void CMainWindow::showDump ()
{
  searchAction (false);
  static int index = 0;
  int        newIndex;
  if (ui->m_stackedWidget->currentIndex () != Dump)
  {
    index    = ui->m_stackedWidget->currentIndex ();
    newIndex = Dump;
  }
  else
  {
    newIndex = index;
  }

  ui->m_stackedWidget->setCurrentIndex (newIndex);
}

void CMainWindow::on_m_provider_textChanged (QString const & text)
{
  if (!ui->m_provider->isReadOnly () && !text.isEmpty ())
  {
    QList<CDidlItem>          items;
    CContentDirectoryBrowser* listWidget = ui->m_stackedWidget->currentWidget ()->findChild<CContentDirectoryBrowser*> ();
    if (listWidget != nullptr)
    {
      int cItems = listWidget->count ();
      items.reserve (cItems);
      for (int iItem = 0; iItem < cItems; ++iItem)
      {
        CContentDirectoryBrowserItem* item = static_cast<CContentDirectoryBrowserItem*>(listWidget->item (iItem));
        items.push_back (item->didlItem ());
      }

      listWidget->clear ();
      items = CBrowseReply::search (items, text);
      if (!items.isEmpty ())
      {
        listWidget->addItems (items);
        listWidget->scrollToTop ();
      }
    }
  }
}

void CMainWindow::search ()
{
  CContentDirectoryBrowser* listWidget = ui->m_stackedWidget->currentWidget ()->findChild<CContentDirectoryBrowser*> ();
  if (listWidget != nullptr)
  {
    QList<QAction*> actions = ui->m_contextualActions->actions ();
    for (QAction* action : actions)
    {
      if (action->data ().toInt () == Search)
      {
        contextualAction (action);
        break;
      }
    }
  }
}

void CMainWindow::networkComStarted (CDevice::EType type)
{
  CNetworkProgress* progress = networkProgress (type);
  if (progress != nullptr && progress != nullptr)
  {
    progress->start ();
  }
}

void CMainWindow::networkComEnded (CDevice::EType type)
{
  CNetworkProgress* progress = this->networkProgress (type);
  if (progress != nullptr && !progress->hasSkipStop ())
  {
    progress->stop ();
  }
}

