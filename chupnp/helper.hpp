#ifndef HELPER_HPP
#define HELPER_HPP

#include <QTime>

class QTreeWidgetItem;
class QColor;
class QTextStream;

bool isHandled (QString const & name);
void addElapsedTime (QTreeWidgetItem* item);
void setGetActionItemColor (QTreeWidgetItem* item);
void setItemColor (QTreeWidgetItem* item, QColor const & color);
int timeMs (QTime const & time);
int timeMs (QString const & time);
void enumerateChildren (QTextStream& s, QTreeWidgetItem* item);
void addTotalTime (QTreeWidgetItem* item, int ms);

#endif // HELPER_HPP
