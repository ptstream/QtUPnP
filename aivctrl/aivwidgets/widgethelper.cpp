#include "widgethelper.hpp"
#include <QDialog>
#include <QDir>
#include <QListWidget>

static QDir s_iconDirRes (":/icons");
static QString s_iconSuffixRes (".png");

void setTransparentBackGround (QWidget* widget)
{
  QWidget* parent = widget->parentWidget ();
  if (parent != nullptr)
  {
    QColor   color   = parent->palette ().color (parent->backgroundRole ());
    QPalette palette = widget->palette ();
    palette.setColor (QPalette::Base, color);
    widget->setPalette (palette);
  }
}

void setItemMouseOverColor (QListWidget* widget)
{
  QPalette palette           = widget->palette ();
  QColor   itemSelectedColor = palette.color(QPalette::Highlight);
  itemSelectedColor.setAlpha (164);
  QString  styleSheet = QString ("QListView::item:hover {background: #%1;}").arg (itemSelectedColor.rgba (), 0, 16);
  widget->setStyleSheet (styleSheet);
}

void removeWindowContextHelpButton (QDialog* dialog)
{
  Qt::WindowFlags flags = dialog->windowFlags ();
  flags                = flags & (~Qt::WindowContextHelpButtonHint);
  dialog->setWindowFlags (flags);
}

QString tooltipField (QString const & title, QString const & value, bool first)
{
  QString string;
  if (!value.isEmpty ())
  {
    string = first ? QString ("<b>%1: %2</b>").arg (title, value) :
                     QString ("<br>%1: %2").arg (title, value);
  }

  return string;
}

QString tooltipField (QString const & title, unsigned value, bool first)
{
  QString string;
  if (value != 0)
  {
    string = tooltipField (title, QString::number (value), first);
  }

  return string;
}

QString tooltipField (QString const & title, double value, bool first)
{
  QString string;
  if (value != 0)
  {
    string = tooltipField (title, QString::number (value), first);
  }

  return string;
}

void setIconDirRes (QString const & dir)
{
  s_iconDirRes.setPath (dir);
}

void setIconSuffixRes (QString const & suffix)
{
  s_iconSuffixRes = suffix;
}

QString resIconFullPath (QString const & name)
{
  QFileInfo fi (s_iconDirRes, name);
  if (fi.suffix ().isEmpty ())
  {
    fi.setFile (s_iconDirRes, name + s_iconSuffixRes);
  }

  return fi.absoluteFilePath ();
}

QIcon resIcon (QString const & name)
{
  return QIcon (resIconFullPath (name));
}

QString updateName (QString name, QStringList const & existingNames)
{
  if (std::find (existingNames.cbegin (), existingNames.cend (), name) != existingNames.cend ())
  {
    QRegExp const rx ("-([0-9]+)");
    int           index = name.lastIndexOf (rx);
    if (index != -1)
    {
      name.truncate (index);
    }

    int indexMax = -1;
    for (QString const & existingName : existingNames)
    {
      QString simplifiedName = existingName;
      index                  = existingName.lastIndexOf (rx);
      if (index != -1)
      {
        simplifiedName.truncate (index);
      }

      if (simplifiedName == name)
      {
        if (indexMax == -1)
        {
          indexMax = 1;
        }

        QString number = existingName.mid (index);
        number.remove ('-');
        int index = number.toInt ();
        if (index > indexMax)
        {
          indexMax = index;
        }
      }
    }

    if (indexMax != -1)
    {
      ++indexMax;
      name += QString ("-%1").arg (indexMax);
    }
  }

  return name;
}
