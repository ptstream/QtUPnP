#-------------------------------------------------
#
# Project created by QtCreator 2017-06-23T09:07:37
#
#-------------------------------------------------

QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = chupnp
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    helper.cpp \
    sendgetaction.cpp \
    loadservices.cpp \
    slots.cpp \
    item.cpp \
    actionParams.cpp \
    stdaction.cpp \
    actiontriggered.cpp \
    search.cpp

HEADERS  += mainwindow.hpp \
    helper.hpp \
    item.hpp \
    actionParams.hpp \
    stdaction.hpp \
    search.hpp

FORMS    += mainwindow.ui \
    actionParams.ui \
    stdaction.ui \
    search.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../upnp/release/ -lqtupnp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../upnp/debug/ -lqtupnp

unix {
  LIBS += $$OUT_PWD/../upnp/libqtupnp.a
  PRE_TARGETDEPS += $$OUT_PWD/../upnp/libqtupnp.a
}

RESOURCES += \
    chupnp.qrc
