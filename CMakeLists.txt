cmake_minimum_required(VERSION 3.24)

project(feeder LANGUAGES CXX)

add_subdirectory(GCNWiiUFeeder)
add_subdirectory(thirdparty)
add_subdirectory(ViGEmLinux)

add_executable(feeder GCNWiiUFeeder/main.cpp)
target_link_libraries(feeder GCNWiiUFeeder libusb yamlcpp vigem)
