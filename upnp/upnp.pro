#-------------------------------------------------
#
# Project created by QtCreator 2016-01-07T11:47:30
#
#-------------------------------------------------

QT += gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11


TARGET = qtupnp
TEMPLATE = lib


win32 {
    DEFINES += UPNP_EXPORTS _WIN32_WINNT=0x0600
}

SOURCES += \
    actionmanager.cpp \
    controlpoint.cpp \
    datacaller.cpp \
    devicemap.cpp \
    helper.cpp \
    initialdiscovery.cpp \
    multicastsocket.cpp \
    unicastsocket.cpp \
    upnpsocket.cpp \
    httpparser.cpp \
    waitingloop.cpp \
    didlitem.cpp \
    xmlh.cpp \
    xmlhdidllite.cpp \
    browsereply.cpp \
    connectioninfo.cpp \
    transportinfo.cpp \
    transportsettings.cpp \
    mediainfo.cpp \
    positioninfo.cpp \
    devicecaps.cpp \
    devicepixmap.cpp \
    xmlhdevice.cpp \
    device.cpp \
    service.cpp \
    statevariable.cpp \
    action.cpp \
    xmlhservice.cpp \
    argument.cpp \
    avtransport.cpp \
    connectionmanager.cpp \
    renderingcontrol.cpp \
    contentdirectory.cpp \
    avcontrol.cpp \
    eventingmanager.cpp \
    xmlhevent.cpp \
    actioninfo.cpp \
    xmlhaction.cpp \
    control.cpp \
    didlitem_playlist.cpp \
    httpserver.cpp \
    pixmapcache.cpp \
    dump.cpp

HEADERS +=\
        upnp_global.hpp \
    actionmanager.hpp \
    controlpoint.hpp \
    datacaller.hpp \
    devicemap.hpp \
    helper.hpp \
    initialdiscovery.hpp \
    multicastsocket.hpp \
    unicastsocket.hpp \
    upnpsocket.hpp \
    using_upnp_namespace.hpp \
    httpparser.hpp \
    waitingloop.hpp \
    didlitem.hpp \
    xmlh.hpp \
    xmlhdidllite.hpp \
    browsereply.hpp \
    connectioninfo.hpp \
    transportinfo.hpp \
    transportsettings.hpp \
    mediainfo.hpp \
    positioninfo.hpp \
    devicecaps.hpp \
    status.hpp \
    devicepixmap.hpp \
    xmlhdevice.hpp \
    device.hpp \
    service.hpp \
    statevariable.hpp \
    action.hpp \
    xmlhservice.hpp \
    argument.hpp \
    avtransport.hpp \
    connectionmanager.hpp \
    control.hpp \
    renderingcontrol.hpp \
    contentdirectory.hpp \
    avcontrol.hpp \
    eventingmanager.hpp \
    xmlhevent.hpp \
    actioninfo.hpp \
    xmlhaction.hpp \
    httpserver.hpp \
    pixmapcache.hpp \
    dump.hpp

unix {
    CONFIG += staticlib
}

