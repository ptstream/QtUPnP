#ifndef LANGUAGE_HPP
#define LANGUAGE_HPP

#include <QDialog>

class QListWidgetItem;
class QAbstractButton;

namespace Ui {
  class CLanguage;
}

/*! This dialog provides functionalities to change the current language. */
class CLanguage : public QDialog
{
  Q_OBJECT

public:
  enum EType { tyLongName, tyShortName, tyQmFile };
  typedef std::array<QString, 3> TLanguage;

  explicit CLanguage (QWidget* parent = nullptr);
  ~CLanguage ();

  QString qmFileSelected () const;
  QString shortNameSelected () const;

protected slots :
  void on_m_languages_itemDoubleClicked (QListWidgetItem* item);
  void on_m_languages_itemSelectionChanged ();
  void on_m_ok_clicked ();
  void on_m_cancel_clicked ();

private:
  Ui::CLanguage*   ui;
  QList<TLanguage> m_availableLanguages;
  int              m_slectedLanguageIndex = -1;
};

#endif // LANGUAGE_HPP
