#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "item.hpp"
#include "../upnp/avcontrol.hpp"
#include "../upnp/httpserver.hpp"
#include "../upnp/pixmapcache.hpp"

#include <QMainWindow>
#include <QTimer>
#include <QStringList>
#include <QMap>
#include <QBrush>
#include <QTreeWidgetItem>

namespace Ui {
  class CMainWindow;
}

namespace upnp
{
 class CControlPoint;
};

class QProgressBar;

USING_UPNP_NAMESPACE

class CMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  // To store some cuttent values.
  struct SCurrent
  {
    void reset ()
    {
      m_vol = 0, m_volMin = 0, m_volMax = 0, m_volStep = 1;
      m_brightness = m_contrast = m_sharpness = m_redVideoGain = m_redVideoBlackLevel = m_greenVideoGain =
      m_greenVideoBlackLevel = m_blueVideoGain = m_blueVideoBlackLevel = m_colorTemperature =
      m_horizontalKeystone = m_verticalKeystone = m_volDB = 0, m_volMinDB = 0, m_volMaxDB = 0;
      m_loudness = m_mute = false;
      m_avTransportURI.clear ();
      m_nextAVTransportURI.clear ();
      m_transportInfo.clear ();
      m_transportSettings.clear ();
      m_playModes.clear ();
      m_searchCaps.clear ();
      m_relTime.clear ();
      m_duration.clear ();
      m_criteria.clear ();
      m_preset.clear ();
      m_presets.clear ();
    }

    int m_vol = 0, m_volMin = 0, m_volMax = 0, m_volStep = 1;
    int m_brightness = 0;
    int m_contrast = 0;
    int m_sharpness = 0;
    int m_redVideoGain = 0;
    int m_redVideoBlackLevel = 0;
    int m_greenVideoGain = 0;
    int m_greenVideoBlackLevel = 0;
    int m_blueVideoGain = 0;
    int m_blueVideoBlackLevel = 0;
    int m_colorTemperature = 0;
    int m_horizontalKeystone = 0;
    int m_verticalKeystone = 0;
    int m_volDB = 0, m_volMinDB = 0, m_volMaxDB = 0;
    bool m_loudness = false;
    bool m_mute = false;
    QString m_avTransportURI;
    QString m_nextAVTransportURI;
    QString m_transportInfo;
    QString m_transportSettings;
    QStringList m_playModes;
    QStringList m_searchCaps;
    QString m_relTime, m_duration;
    QString m_criteria;
    QString m_preset;
    QStringList m_presets;
  };

  explicit CMainWindow (QWidget* parent = 0);
  ~CMainWindow ();

protected :
  virtual void closeEvent (QCloseEvent*); // See Detailed Description of CControlPoint

protected slots :
  void newDevice (QString const & uuid);
  void lostDevice (QString const & uuid);
  void eventReady (QStringList const & emitter);
  void upnpError (int errorCode, QString const & errorString);
  void networkError (QString const & deviceUUID, QNetworkReply::NetworkError errorCode, QString const & errorString);
  void discoveryLaunched (char const * nt, int index, int count);
  void timeout ();
  void on_m_rescan_triggered ();
  void on_m_update_triggered ();
  void on_m_export_triggered ();
  void on_m_copyURI_triggered ();
  void on_m_playlist_triggered ();
  void on_m_search_triggered ();
  void on_m_devices_itemDoubleClicked (QTreeWidgetItem* item, int column);
  void on_m_services_itemDoubleClicked (QTreeWidgetItem* item, int column);
  void on_m_services_itemClicked (QTreeWidgetItem* item, int column);

private :
  QTreeWidgetItem* insertItem (QTreeWidgetItem* parentItem, CDevice const & device);
  void createActionLinks ();
  void hideProgressBar (bool hide);
  void loadServices (QString const & uuid);
  bool subscription(QString const & uuid);
  void sendGetAction (QTreeWidgetItem* item);
  void updateGetAction (QString const & name);
  QTreeWidgetItem* findActionItem (QString const & text);
  void removeActionArgs (QTreeWidgetItem* item);
  void applyError (QTreeWidgetItem* item);
  void clearError (QTreeWidgetItem* item);
  void browse (QTreeWidgetItem* item);
  void search (QTreeWidgetItem* item, QString const & criteria);
  void updateTree (QTreeWidgetItem* item, CBrowseReply const & reply, EItemType type);
  void insertDidlElems (CItem* item);

private:
  Ui::CMainWindow* ui;
  CControlPoint* m_cp = nullptr; //!< It is the CControlPoint.
  CAVControl m_ctl; //!< AV manager action (just an orginazation).
  QTimer m_timer; //!< Use to start discovery and update progress bar during disconery.
  QProgressBar* m_pb = nullptr; //!< Progress bar during discovery.
  bool m_initRunning = true; //!< Discovery running.
  QString m_deviceUUID; //!< The current device UUID.
  QMap<QString, QStringList> m_actionLinks; //!< Links between actions. e.g setMute expand mute.
  SCurrent m_current; //!< Some current variable values.
  QList<CDidlItem> m_selectedTracks; //!< Use to create playlist.
  QBrush m_itemBrush; //!< Default QTreeWidgetItem color
  int  m_version = 100; //!< Version of this code.
  CPixmapCache m_pxmCache; //!< Cache of pixmaps.
};

#endif // MAINWINDOW_HPP
