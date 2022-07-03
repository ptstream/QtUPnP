cmake_minimum_required(VERSION 3.16)

project(upnp)

find_package(Qt6 COMPONENTS Core Network Xml Core5Compat Gui REQUIRED)

option(USEHIDDENFILE OFF)

set(CMAKE_AUTOMOC ON)

set(upnp_src
  actionmanager.cpp
  controlpoint.cpp
  datacaller.cpp
  devicemap.cpp
  helper.cpp
  initialdiscovery.cpp
  multicastsocket.cpp
  unicastsocket.cpp
  upnpsocket.cpp
  httpparser.cpp
  waitingloop.cpp
  didlitem.cpp
  xmlh.cpp
  xmlhdidllite.cpp
  browsereply.cpp
  connectioninfo.cpp
  transportinfo.cpp
  transportsettings.cpp
  mediainfo.cpp
  positioninfo.cpp
  devicecaps.cpp
  devicepixmap.cpp
  xmlhdevice.cpp
  device.cpp
  service.cpp
  statevariable.cpp
  action.cpp
  xmlhservice.cpp
  argument.cpp
  avtransport.cpp
  connectionmanager.cpp
  renderingcontrol.cpp
  contentdirectory.cpp
  avcontrol.cpp
  eventingmanager.cpp
  xmlhevent.cpp
  actioninfo.cpp
  xmlhaction.cpp
  control.cpp
  didlitem_playlist.cpp
  httpserver.cpp
  pixmapcache.cpp
  dump.cpp
  plugin.cpp
  oauth2.cpp
  aesencryption.cpp)


set(upnp_header
aes256.h
aesencryption.h
avcontrol.hpp
connectionmanager.hpp
didlitem.hpp
eventingmanager.hpp
initialdiscovery.hpp
multicastsocket.hpp
pixmapcache.hpp
statevariable.hpp
unicastsocket.hpp
using_upnp_namespace.hpp
xmlh.hpp
xmlhaction.hpp
xmlhdevice.hpp
xmlhdidllite.hpp
xmlhevent.hpp
xmlhservice.hpp)


set(upnp_public_header
  action.hpp
  actioninfo.hpp
  actionmanager.hpp
  argument.hpp
  avtransport.hpp
  browsereply.hpp
  connectioninfo.hpp
  contentdirectory.hpp
  control.hpp
  controlpoint.hpp
  datacaller.hpp
  device.hpp
  devicecaps.hpp
  devicemap.hpp
  devicepixmap.hpp
  dump.hpp
  helper.hpp
  httpparser.hpp
  httpserver.hpp
  mediainfo.hpp
  oauth2.hpp
  plugin.hpp
  positioninfo.hpp
  renderingcontrol.hpp
  service.hpp
  status.hpp
  transportinfo.hpp
  transportsettings.hpp
  upnp_global.hpp
  upnpsocket.hpp
  waitingloop.hpp
 )

if(UNIX)
  add_library(upnp STATIC ${upnp_header} ${upnp_public_header} ${upnp_src})
else()
  add_library(upnp SHARED ${upnp_header} ${upnp_public_header} ${upnp_src})
endif()

target_link_libraries(upnp PRIVATE Qt6::Core Qt6::Xml Qt6::Network Qt6::Core5Compat Qt6::Gui)
set_target_properties(upnp PROPERTIES PUBLIC_HEADER "${upnp_public_header}")

if(WIN32)
  target_compile_definitions(upnp PUBLIC UPNP_EXPORTS _WIN32_WINNT=0x0600)
endif()

if(USEHIDDENFILE)
  target_compile_definitions(upnp PUBLIC HIDDENFILES=1)
endif()
