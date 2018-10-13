#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <QDialog>

namespace Ui {
  class CSettings;
}

class CSettings : public QDialog
{
  Q_OBJECT
public:
  enum EIndex { ShowNetworkCom,
                UseSearchForCheckPlaylist,
                UPnPEventsOnly,
                DontUsePlaylists,
                ShowCloudServers,
                LastIndex,
              };

  explicit CSettings (bool* flags, QSize& iconSize,  QWidget* parent = nullptr);
  ~CSettings ();

  static void setIconSize (QWidget* w, QSize const & size);

private slots :
  void on_m_reset_clicked ();
  void on_m_ok_clicked ();
  void on_m_cancel_clicked ();
  void on_m_iconSize_valueChanged (int value);

private:
  Ui::CSettings* ui;
  bool* m_flags;
  QSize& m_iconSize;
  QSize m_iconSizeBackup;
};

#endif // SETTINGS_HPP
