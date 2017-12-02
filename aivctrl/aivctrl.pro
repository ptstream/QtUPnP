#-------------------------------------------------
#
# Project created by QtCreator 2017-09-22T17:51:22
#
#-------------------------------------------------

QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = aivctrl
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    systemevents.cpp \
    icons.cpp \
    mainwindowhelper.cpp \
    popupmenus.cpp \
    buttonslots.cpp \
    listwidgetslots.cpp \
    variousslots.cpp \
    controlpointslots.cpp \
    helper.cpp \
    sliderslots.cpp \
    session.cpp \
    playlist.cpp \
    playlistchoice.cpp \
    xmlhplaylist.cpp \
    serverscanner.cpp \
    about.cpp \
    language.cpp \
    settings.cpp \
    aivwidgets/contentdirectorybrowser.cpp \
    aivwidgets/cover.cpp \
    aivwidgets/devicebrowser.cpp \
    aivwidgets/folderbrowser.cpp \
    aivwidgets/listwidgetbase.cpp \
    aivwidgets/mydevicebrowser.cpp \
    aivwidgets/networkprogress.cpp \
    aivwidgets/playlistbrowser.cpp \
    aivwidgets/position.cpp \
    aivwidgets/slider.cpp \
    aivwidgets/textmetadata.cpp \
    aivwidgets/widgethelper.cpp

HEADERS  += mainwindow.hpp \
    helper.hpp \
    session.hpp \
    playlist.hpp \
    playlistchoice.hpp \
    xmlhplaylist.hpp \
    serverscanner.hpp \
    about.hpp \
    language.hpp \
    settings.hpp \
    aivwidgets/contentdirectorybrowser.hpp \
    aivwidgets/cover.hpp \
    aivwidgets/devicebrowser.hpp \
    aivwidgets/folderbrowser.hpp \
    aivwidgets/listwidgetbase.hpp \
    aivwidgets/mydevicebrowser.hpp \
    aivwidgets/networkprogress.hpp \
    aivwidgets/playlistbrowser.hpp \
    aivwidgets/position.hpp \
    aivwidgets/slider.hpp \
    aivwidgets/textmetadata.hpp \
    aivwidgets/widgethelper.hpp

FORMS    += mainwindow.ui \
    playlistchoice.ui \
    about.ui \
    language.ui \
    settings.ui

RESOURCES += \
    aivctrl.qrc

DISTFILES += \
    aivctrl.rc

RC_FILE += \
    aivctrl.rc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../upnp/release/ -lqtupnp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../upnp/debug/ -lqtupnp

unix {
  LIBS += $$OUT_PWD/../upnp/libqtupnp.a
  PRE_TARGETDEPS += $$OUT_PWD/../upnp/libqtupnp.a
}

