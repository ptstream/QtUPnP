#include "mainwindow.hpp"
#include "../upnp/actionmanager.hpp"
#include <QTreeWidgetItem>

static char const * actions[] = { "GetMute",
                                  "SetMute",
                                  "GetVolume",
                                  "SetVolume",
                                  "GetCurrentConnectionIDs",
                                  "GetCurrentConnectionInfo",
                                  "GetProtocolInfo",
                                  "GetCurrentTransportActions",
                                  "GetDeviceCapabilities",
                                  "GetMediaInfo",
                                  "GetPositionInfo",
                                  "GetTransportInfo",
                                  "GetTransportSettings",
                                  "Next",
                                  "Pause",
                                  "Play",
                                  "Previous",
                                  "Seek",
                                  "SetAVTransportURI",
                                  "SetNextAVTransportURI",
                                  "SetPlayMode",
                                  "Stop",
                                  "Browse",
                                  "GetSearchCapabilities",
                                  "GetSortCapabilities",
                                  "GetSystemUpdateID",
                                  "ListPresets",
                                  "SelectPreset",
                                  "Search",
                                  "GetBrightness",
                                  "GetContrast",
                                  "GetSharpness",
                                  "GetRedVideoGain",
                                  "GetRedVideoBlackLevel",
                                  "GetGreenVideoGain",
                                  "GetGreenVideoBlackLevel",
                                  "GetBlueVideoGain",
                                  "GetBlueVideoBlackLevel",
                                  "GetColorTemperature",
                                  "GetHorizontalKeystone",
                                  "GetVerticalKeystone",
                                  "GetVolumeDB",
                                  "GetVolumeDBRange",
                                  "GetLoudness",
                                  "SetBrightness",
                                  "SetContrast",
                                  "SetSharpness",
                                  "SetRedVideoGain",
                                  "SetRedVideoBlackLevel",
                                  "SetGreenVideoGain",
                                  "SetGreenVideoBlackLevel",
                                  "SetBlueVideoGain",
                                  "SetBlueVideoBlackLevel",
                                  "SetColorTemperature",
                                  "SetHorizontalKeystone",
                                  "SetVerticalKeystone",
                                  "SetVolumeDB",
                                  "SetLoudness",
                                };

bool isHandled (QString const & actionName)
{
  bool success = false;
  for (char const * action : actions)
  {
     if (actionName == action)
     {
       success = true;
       break;
     }
  }

  return success;
}


void addElapsedTime (QTreeWidgetItem* item)
{
  QString text = item->text (0) + " (" + QString::number (CActionManager::lastElapsedTime ()) + "ms)";
  item->setText (0, text);
}

void setGetActionItemColor (QTreeWidgetItem* item)
{
  QBrush brush (QColor (0, 0, 255));
  item->setForeground (0, brush);
}

void setItemColor (QTreeWidgetItem* item, QColor const & color)
{
  QBrush brush (color);
  item->setForeground (0, brush);
}

int timeMs (QTime const & time)
{
  int duration = (time.hour () * 3600 + time.minute () * 60 + time.second ()) * 1000 + time.msec ();
  return duration;
}

int timeMs (QString const & time)
{
  QTime ti = QTime::fromString (time);
  return timeMs (ti);
}

void enumerateChildren (QTextStream& s, QTreeWidgetItem* item)
{
  s << "<ul>";
  for (int iChild = 0; iChild < item->childCount (); ++iChild)
  {
    QTreeWidgetItem* childItem = item->child (iChild);
    QBrush           brush     = childItem->foreground (0);
    QColor           color     = brush.color ();
    QString          rgb;
    if (color.red () != 0 || color.green () != 0 || color.blue () != 0)
    {
      rgb = QString ("<font color=\"#%1%2%3\">").arg (color.red (), 2, 16, QChar ('0'))
                                                .arg (color.green (), 2, 16, QChar ('0'))
                                                .arg (color.blue (), 2, 16, QChar ('0'));
      s << rgb;
    }

    QString const & text= childItem->text (0);
    s << "<li>" << text << "</li>";
    if (!rgb.isEmpty ())
    {
      s << "</font>";
    }

    if (childItem->childCount () != 0)
    {
      enumerateChildren (s, childItem);
    }
  }

  s << "</ul>";
}

void addTotalTime (QTreeWidgetItem* item, int ms)
{
  QString text = item->text (0);
  text        += " (" + QString::number (ms / 1000.) + "s)";
  item->setText (0, text);
}
