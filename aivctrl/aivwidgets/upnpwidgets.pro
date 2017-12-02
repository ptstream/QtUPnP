#-------------------------------------------------
#
# Project created by QtCreator 2017-09-21T14:12:21
#
#-------------------------------------------------

QT       += widgets network

TARGET = upnpwidgets
TEMPLATE = lib

DEFINES += UPNPWIDGETS_LIBRARY

win32 {
  DEFINES += UPNP_EXPORTS _WIN32_WINNT=0x0600
  CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../upnp/release/ -llibqtupnp
  else:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../upnp/debug/ -llibqtupnp
}

unix {
  LIBS += $$OUT_PWD/../upnp/libqtupnp.a
  PRE_TARGETDEPS += $$OUT_PWD/../upnp/libqtupnp.a
}

SOURCES += \
    contentdirectorybrowser.cpp \
    devicebrowser.cpp \
    playlistbrowser.cpp \
    folderbrowser.cpp \
    listwidgetbase.cpp \
    widgethelper.cpp \
    textmetadata.cpp \
    cover.cpp \
    position.cpp \
    slider.cpp \
    mydevicebrowser.cpp \
    networkprogress.cpp

HEADERS +=\
        upnpwidgets_global.hpp \
    contentdirectorybrowser.hpp \
    devicebrowser.hpp \
    playlistbrowser.hpp \
    folderbrowser.hpp \
    listwidgetbase.hpp \
    widgethelper.hpp \
    textmetadata.hpp \
    cover.hpp \
    position.hpp \
    slider.hpp \
    mydevicebrowser.hpp \
    networkprogress.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}
