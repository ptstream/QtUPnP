#ifndef ACTIONPARAMS_HPP
#define ACTIONPARAMS_HPP

#include <QDialog>

namespace Ui {
  class CActionParams;
}

/*! \brief The dialog to enter in parameters, launch an UPnP:AV action.
 *
 * The result is in the tree.
 */
class CActionParams : public QDialog
{
  Q_OBJECT

public:
  enum EStatus { stNoRange, stString, stInteger, stBoolean };
  explicit CActionParams (QWidget* parent = 0);
  ~CActionParams ();

  static void setHidden (QList<QWidget*> const & ws, bool hide = true);
  static bool sendMute (bool& current);
  static bool sendVolume(int& current, int min, int max, int step);
  static bool sendVolumeDB (int& current, int min, int max);
  static bool sendAVTransportURI (QString& current);
  static bool sendNextAVTransportURI (QString& current);
  static bool sendPPS (QString const & transportState, QString const & action); // PPS = play-pause-stop
  static bool sendPN (QString const & action); // PN = previous-next
  static bool sendPlayMode (QString& current, QStringList const & allowed);
  static bool sendSeek (QString& current, QString const & trackTime);
  static bool sendSearch (QString& criteria, QStringList const & allowed);
  static bool selectPreset (QString& preset, QStringList const & allowed);

protected slots :
  void on_m_allowed_currentTextChanged (QString const & text);
  void on_m_copy_clicked ();

private:
  Ui::CActionParams* m_ui;
};

#endif // ACTIONPARAMS_HPP
