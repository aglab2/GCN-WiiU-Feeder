cmake_minimum_required(VERSION 3.24)
project(vigem LANGUAGES C)

add_library(vigem STATIC ViGEm.c)
target_include_directories(vigem PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/ViGEmClient/include)
