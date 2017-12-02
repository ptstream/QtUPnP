#ifndef HELPER_HPP
#define HELPER_HPP

#include <QString>
class QTranslator;

/*! The uniq AIVCtrl identifier. */
#define APPURN "urn:aivctrl:16f0fcf7-a61a-49b9-888e-446322f61113"

/*! Returns the system temp directory. */
QString tempDirectory ();

/*! Returns the application data directory. */
QString appDataDirectory ();

/*! Return "aivctrl". */
QString appBaseName ();

/*! Return application file path. For example for Windows, C:\Program Files (x86)\aivctrl. */
QString appFilePath ();

/*! Return application file path. For example for Windows, C:\Program Files (x86). */
QString appFileDir ();

/*! Returns error file path. */
QString errorFilePath ();

/*! Returns the error file path. For Windows, "C:\Users\%USERNAME%\AppData\Local\Temp\-dump-errors.txt". */
void dumpError (QString const & text);

/*! Removes the error file . */
void removeDumpError ();

/*! Installs a new translator. */
QTranslator* installTranslator (QString& locale);

#endif // HELPER_HPP
