#ifndef WIDGETHELPER_HPP
#define WIDGETHELPER_HPP

#include <QIcon>

class QWidget;
class QDialog;
class QStringList;
class QString;

/*! \brief This file contains functions to refractorize the code. */

/*! Calls tooltipFied as being the first field of the tooltip. */
#define TOOLTIPFIELD1(title,value) tooltipField (QObject::tr (title), value, true)

/*! Calls tooltipFied as being the non first field of the tooltip. */
#define TOOLTIPFIELD(title,value) tooltipField (QObject::tr (title), value, false)

/*! Sets backgroung widget transparent. */
void setTransparentBackGround (QWidget* widget);

/*! Removes the windows context help button. */
void removeWindowContextHelpButton (QDialog* dialog);

/*! Returns the field value.
 * \param title: The title.
 * \param value: The QString value.
 * \param first: True if this field is the first field.
 * \return The field value.
 */
QString tooltipField (QString const & title, QString const & value, bool first = false);

/*! Same as above with integer value. */
QString tooltipField (QString const & title, unsigned value, bool first = false);

/*! Same as above with double value. */
QString tooltipField (QString const & title, double value, bool first = false);

/*! Returns the icon full path. */
QString resIconFullPath (QString const & name);

/*! Returns the icon from name. */
QIcon resIcon (QString const & name);

/*! Sets the icon directory. By default ":/icons for a resource. */
void setIconDirRes (QString const & dir);

/*! Sets the icon suffix. By default ".png". */
void setIconSuffixRes (QString const & suffix);

/*! Updates name.
 * From a list of existing names, this function returns a new name.
 * For example, if name is "New playlist", and if "New playlist" exists in existingNames,
 * The function returns "New playlist-2". If "New playlist-2" exists, the return is "New playlist-3".
 * and so on.
 */
QString updateName (QString name, QStringList const & existingNames);

#endif // WIDGETHELPER_HPP
