cmake_minimum_required(VERSION 3.16)

project(chupnp)


set(CMAKE_CURRENT_BINARY_DIR ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Network Xml Core5Compat Gui Widgets)

set(source main.cpp
  mainwindow.cpp
  helper.cpp
  sendgetaction.cpp
  loadservices.cpp
  slots.cpp
  item.cpp
  actionParams.cpp
  stdaction.cpp
  actiontriggered.cpp
  search.cpp)

set(headers mainwindow.hpp
    helper.hpp
    item.hpp
    actionParams.hpp
    stdaction.hpp
    search.hpp)

set(ui mainwindow.ui
    actionParams.ui
    stdaction.ui
    search.ui)


set(resources chupnp.qrc)

add_executable(chupnp ${source} ${headers} ${ui} ${resources})
target_link_libraries(chupnp PRIVATE Qt6::Widgets Qt6::Core Qt6::Xml Qt6::Network Qt6::Core5Compat Qt6::Gui upnp)

