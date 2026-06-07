cmake_minimum_required(VERSION 3.24)
project(GCNWiiUFeeder LANGUAGES CXX)

set(GCN_SOURCES
    ControllerInterface.cpp
    debug.cpp
    EmuController.cpp
    GCN.cpp
    GCNController.cpp
    GenericController.cpp
    Mapping.cpp
    PlatformLinux.cpp
    Serialization.cpp
    Usb.cpp
    X360Controller.cpp
)

add_library(GCNWiiUFeeder STATIC ${GCN_SOURCES})
target_include_directories(GCNWiiUFeeder PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/thirdparty/ViGEmClient/include
    ${CMAKE_SOURCE_DIR}/thirdparty/libusb
    ${CMAKE_SOURCE_DIR}/thirdparty/yaml-cpp/include
)
