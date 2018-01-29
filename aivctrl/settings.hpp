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
                LastIndex,
              };

  explicit CSettings (bool* flags, QWidget* parent = 0);
  ~CSettings ();

private slots :
  void on_m_reset_clicked ();
  void on_m_ok_clicked ();
  void on_m_cancel_clicked ();

private:
  Ui::CSettings* ui;
  bool* m_flags;
};

#endif // SETTINGS_HPP
