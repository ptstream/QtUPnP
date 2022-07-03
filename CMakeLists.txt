cmake_minimum_required(VERSION 3.16)

project(QtUPnp)


set(CMAKE_CURRENT_BINARY_DIR ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_subdirectory(upnp)
add_subdirectory(chupnp)
