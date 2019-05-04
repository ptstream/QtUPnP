#-------------------------------------------------
#
# Project created by QtCreator 2017-12-12T15:17:56
#
#-------------------------------------------------

QT       = gui network xml

TARGET = googledrive
TEMPLATE = lib

DEFINES += GOOGLEDRIVE_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += pluginobject.cpp \
    browse.cpp

HEADERS += pluginobject.hpp\
        googledrive_global.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../upnp/release/ -lqtupnp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../upnp/debug/ -lqtupnp

unix {
  LIBS += $$OUT_PWD/../upnp/libqtupnp.a
  PRE_TARGETDEPS += $$OUT_PWD/../upnp/libqtupnp.a
}

# Copy files needed for plugin (e.g. googledrive.ids, googledrive.png, googledrive.dll)
# For this dep.cmd is launched after build.
win32 {
CONFIG(release, debug|release): DESTDIR = $$OUT_PWD/../aivctrl/release/plugins/
CONFIG(debug, debug|release): DESTDIR = $$OUT_PWD/../aivctrl/debug/plugins/
PLUGINDESTDIR=$$replace(DESTDIR, /, \\)
PLUGINDIR=$$replace(PWD, /, \\)
QMAKE_POST_LINK =$$PLUGINDIR\dep.cmd $$PLUGINDIR $$PLUGINDESTDIR
}

# Copy files needed for plugin (e.g. googledrive.ids, googledrive.png, googledrive.so)
# For this dep.sh is launched after build.
unix {
CONFIG(release, release): DESTDIR = $$OUT_PWD/../aivctrl/plugins/
CONFIG(debug, debug): DESTDIR = $$OUT_PWD/../aivctrl/plugins/
PLUGINDESTDIR=$$DESTDIR
PLUGINDIR=$$PWD
QMAKE_POST_LINK = $$_PRO_FILE_PWD_/dep.sh $$PLUGINDIR $$PLUGINDESTDIR
}

# Copy files needed for plugin (e.g. googledrive.ids, googledrive.png, googledrive.dylib)
# For this depmac.sh is launched after build.
macx {
CONFIG(release, release): DESTDIR = $$OUT_PWD/../aivctrl/plugins/
CONFIG(debug, debug): DESTDIR = $$OUT_PWD/../aivctrl/plugins/
PLUGINDESTDIR=$$DESTDIR
PLUGINDIR=$$PWD
QMAKE_POST_LINK = $$_PRO_FILE_PWD_/depmac.sh $$PLUGINDIR $$PLUGINDESTDIR
}

DISTFILES += \
    dep.sh \
    depmac.sh \
    googledrive.ids \
    googledrive.png \
    dep.cmd
