#include "helper.hpp"
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QDialog>
#include <QLocale>
#include <QTranslator>

QString tempDirectory ()
{
  return QStandardPaths::standardLocations (QStandardPaths::TempLocation).first ();
}

QString appDataDirectory ()
{
  QString     folder;
  QStringList folders = QStandardPaths::standardLocations (QStandardPaths::AppDataLocation);
  if (!folders.isEmpty ())
  {
    folder = folders.first ();
    QDir appDataFolder (folder);
    if (!appDataFolder.exists (folder))
    {
      appDataFolder.mkpath (folder);
    }
  }

  return folder;
}

QString appBaseName ()
{
  QFileInfo fi (appFilePath ());
  return fi.baseName ();
}

QString appFilePath ()
{
  return QCoreApplication::applicationFilePath ();
}

QString appFileDir ()
{
  QFileInfo fileInfo (appFilePath ());
  return fileInfo.absolutePath ();
}

QString errorFilePath ()
{
  QDir    dir (tempDirectory ());
  QString fileName = appBaseName () + "-dump-errors" + ".txt";
  return dir.absoluteFilePath (fileName);
}

void dumpError (QString const & text)
{
  QFile file (errorFilePath ());
  file.open (QIODevice::Append | QIODevice::Text);
  file.write (text.toLatin1 ());
  file.close ();
}

void removeDumpError ()
{
  QFile::remove (errorFilePath ());
}

QTranslator* installTranslator (QString& locale)
{
  if (locale.isEmpty ())
  {
    locale = QLocale::system ().name ().section ('_', 0, 0);
  }

  QString      path       = appFileDir () + "/languages/aivctrl_" + locale + ".qm";
  QTranslator* translator = nullptr;
  if (QFileInfo::exists (path))
  {
    translator = new QTranslator;
    translator->load (path);
    QCoreApplication::installTranslator (translator);
  }

  return translator;
}

