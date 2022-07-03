#include "mainwindow.hpp"
#include "item.hpp"
#include "helper.hpp"
#include "search.hpp"
#include "ui_mainwindow.h"
#include <QTreeWidget>
#include <QStandardPaths>
#include <QFileDialog>
#include <QClipboard>
#include <QElapsedTimer>

void CMainWindow::on_m_rescan_triggered ()
{
  QApplication::setOverrideCursor (Qt::WaitCursor);
  ui->m_tabWidget->setCurrentIndex (0);
  ui->m_services->clear ();
  hideProgressBar (false);

  QElapsedTimer ti;
  ti.start ();

  m_cp->discover (); // Launch the UPnP device discovery. This function can be called more than once.

  int ms = ti.elapsed ();
  QString message = QString ("Discovery time: %1 s").arg (static_cast<float>(ms) / 1000.0f);
  statusBar ()->showMessage (message);
  hideProgressBar (true);
  QApplication::restoreOverrideCursor ();
}

void CMainWindow::on_m_update_triggered ()
{
  loadServices (m_deviceUUID);
}

void CMainWindow::on_m_export_triggered ()
{
  auto enumerateVariable = [] (QTextStream& s, QTreeWidgetItem* item)
  {
    s << "<table border=\"1\" cellpadding=\"2px;\">";
    for (int iItem = 0; iItem < item->childCount (); ++iItem)
    {
      QTreeWidgetItem* varItem = item->child (iItem);
      s << "<tr>";
      for (int iCol = 0; iCol < varItem->columnCount (); ++iCol)
      {
        QString text = varItem->text (iCol);
        s << "<td>" << text <<  "</td>";
      }

      s << "</tr>";
    }

    s << "</table>";
  };

  QStringList args = QApplication::arguments ();
  QFileInfo   fileInfo (args[0]);
  QString     exeBaseName =  fileInfo.baseName ();

  QDateTime date = QDateTime::currentDateTime ();
  QString caption;
  QStringList dirs = QStandardPaths::standardLocations (QStandardPaths::DocumentsLocation);

  QString fileName = QString ("%1_upnp_services_%2.htm").arg (exeBaseName).arg (date.toString (locale().dateTimeFormat(QLocale::ShortFormat)));
  fileName.replace ('/', '-');
  fileName.replace (':', '-');
  fileName.replace (' ', '_');
  QString     dir  = QDir (dirs.first ()).absoluteFilePath (fileName);
  fileName         = QFileDialog::getSaveFileName (this, caption, dir);
  if (!fileName.isEmpty ())
  {
    QFile file (fileName);
    file.open (QIODevice::WriteOnly | QIODevice::Text);
    QTextStream s (&file);
    s << "<!doctype html><html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/><title>UPnP</title></head><body>";

    int v100   = m_version / 100;
    int v10    = (m_version - v100 * 100) / 10;
    int v1     = (m_version - v100 * 100 - v10 * 10);
    QString v  = QString ("%1.%2.%3").arg (v100).arg (v10).arg (v1);
    QString u  = QString ("<h1 align=\"center\">%1 - %2 (%3)</h1>").arg (tr ("UPnP services")).arg (exeBaseName).arg (v);
    s << u.toUtf8 ();

    u = QString ("<p><font size=\"4\"><b>%1</b></font>%2</p>").arg (tr ("Date: ")).arg (date.toString (locale().dateTimeFormat(QLocale::ShortFormat)));
    s << u.toUtf8 ();

    s << "<ul>";
    for (int i = 0; i < ui->m_services->topLevelItemCount (); ++i)
    {
      QTreeWidgetItem* item = ui->m_services->topLevelItem (i);
      s << "<li>" << item->text (0) << "</li>";

      for (int iChild = 0; iChild < item->childCount (); ++iChild)
      {
        QTreeWidgetItem* childItem = item->child (iChild);
        s << "<ul>";
        QString const & text = childItem->text (0);
        s << "<li>" << text << "</li>";
        if (text == "Variables")
        {
          enumerateVariable (s, childItem);
        }
        else
        {
          enumerateChildren (s, childItem);
        }

        s << "</ul>";
      }
    }

    s << "</ul>";
    s << "</table></body></html>";
  }
}

void CMainWindow::on_m_copyURI_triggered ()
{
  QList<QTreeWidgetItem*> items = ui->m_services->selectedItems ();
  if (!items.isEmpty ())
  {
    on_m_services_itemClicked (items.first (), 0);
  }
}

void CMainWindow::on_m_playlist_triggered ()
{
  m_selectedTracks.clear ();
  QList<QTreeWidgetItem*> items = ui->m_services->selectedItems ();
  if (!items.isEmpty ())
  {
    m_selectedTracks.reserve (items.size ());
    for (QTreeWidgetItem* item : items)
    {
      if (item->type () == BrowseType)
      {
        CDidlItem const & didlItem = static_cast<CItem*>(item)->didl ();
        if (didlItem.type () == CDidlItem::MusicTrack)
        {
          m_selectedTracks.push_back (didlItem);
        }
      }
    }
  }
}

void CMainWindow::on_m_search_triggered ()
{
  QList<QTreeWidgetItem*> items = ui->m_services->selectedItems ();
  QList<CDidlItem>        didlItems;
  didlItems.reserve (items.size ());
  for (QTreeWidgetItem* item : items)
  {
    if (item->type () == BrowseType)
    {
      CDidlItem const & didl = static_cast<CItem*>(item)->didl ();
      if (!didl.isContainer ())
      {
        didlItems.push_back (didl);
      }
    }
  }

  CSearch search (didlItems, this);
  search.exec ();
}
