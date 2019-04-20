#include "language.hpp"
#include "helper.hpp"
#include "ui_language.h"
#include "aivwidgets/widgethelper.hpp"
#include <QDirIterator>
#include <QTranslator>
#include <QPair>
#include <QAbstractButton>
#include <array>

CLanguage::CLanguage (QWidget* parent) : QDialog (parent), ui (new Ui::CLanguage)
{
  ui->setupUi (this);
  removeWindowContextHelpButton (this);
  setTransparentBackGround (ui->m_languages);
  ui->m_ok->setEnabled (false);

  QString qmPath = appFileDir ();
  QDir    qmDir (qmPath);
  qmDir.cd ("languages");

  tr ("French");
  TLanguage   language = { tr ("English"), "en",  QString () };
  m_availableLanguages << language;

  QStringList qmFiles  = qmDir.entryList (QStringList ("*.qm"));
  foreach (QString const & qmFile, qmFiles)
  {
    int      languageIndex  = qmFile.indexOf ('_');
    int      languageSuffix = qmFile.indexOf ('.', languageIndex);
    QString  shortName      = qmFile.mid (languageIndex + 1, languageSuffix - languageIndex - 1);
    QLocale  locale (shortName);
    QString  longName  = QLocale::languageToString (locale.language ());
    TLanguage language = { qApp->translate ("CLanguage", longName.toLatin1 ().constData ()), shortName, qmFile };
    m_availableLanguages << language;
  }

  qSort (m_availableLanguages.begin (), m_availableLanguages.end (), [] (TLanguage const & l1, TLanguage const & l2) -> bool { return l1[tyLongName] < l2[tyLongName]; });

  for (TLanguage const & availableLanguage : m_availableLanguages)
  {
    QListWidgetItem* item = new QListWidgetItem (ui->m_languages);
    item->setText (availableLanguage[tyLongName]);
    QString iconName (qmDir.absoluteFilePath (availableLanguage[tyShortName] + ".png"));
    item->setIcon (QIcon (iconName));
  }
}

CLanguage::~CLanguage ()
{
  delete ui;
}

QString CLanguage::qmFileSelected () const
{
  return m_slectedLanguageIndex > 0 ? m_availableLanguages[m_slectedLanguageIndex][tyQmFile] :  QString ();
}

QString CLanguage::shortNameSelected () const
{
  return m_availableLanguages[m_slectedLanguageIndex][tyShortName];
}

void CLanguage::on_m_languages_itemDoubleClicked (QListWidgetItem* item)
{
  m_slectedLanguageIndex = ui->m_languages->row (item);
  accept ();
}

void CLanguage::on_m_languages_itemSelectionChanged ()
{
  QList<QListWidgetItem*> items = ui->m_languages->selectedItems ();
  ui->m_ok->setEnabled (!items.isEmpty ());
}

void CLanguage::on_m_ok_clicked ()
{
  QList<QListWidgetItem*> items = ui->m_languages->selectedItems ();
  m_slectedLanguageIndex        =  items.isEmpty () ? -1 : ui->m_languages->row (items.first ());
  accept ();
}

void CLanguage::on_m_cancel_clicked ()
{
  reject ();
}
