cmake_minimum_required(VERSION 3.24)
project(thirdparty LANGUAGES C)

add_library(libusb STATIC
    libusb/libusb/core.c
    libusb/libusb/descriptor.c
    libusb/libusb/hotplug.c
    libusb/libusb/io.c
    libusb/libusb/strerror.c
    libusb/libusb/sync.c
    libusb/libusb/os/poll_posix.c
    libusb/libusb/os/threads_posix.c
    libusb/libusb/os/linux_netlink.c
    libusb/libusb/os/linux_udev.c
    libusb/libusb/os/linux_usbfs.c
)

target_compile_definitions(libusb PRIVATE
    -D_GNU_SOURCE
)

target_include_directories(libusb PRIVATE
    libusb/libusb
    config
)

add_library(yamlcpp STATIC
    yaml-cpp/src/binary.cpp
    yaml-cpp/src/convert.cpp
    yaml-cpp/src/directives.cpp
    yaml-cpp/src/emit.cpp
    yaml-cpp/src/emitfromevents.cpp
    yaml-cpp/src/emitter.cpp
    yaml-cpp/src/emitterstate.cpp
    yaml-cpp/src/emitterutils.cpp
    yaml-cpp/src/exceptions.cpp
    yaml-cpp/src/exp.cpp
    yaml-cpp/src/memory.cpp
    yaml-cpp/src/node.cpp
    yaml-cpp/src/node_data.cpp
    yaml-cpp/src/nodebuilder.cpp
    yaml-cpp/src/nodeevents.cpp
    yaml-cpp/src/null.cpp
    yaml-cpp/src/ostream_wrapper.cpp
    yaml-cpp/src/parse.cpp
    yaml-cpp/src/parser.cpp
    yaml-cpp/src/regex_yaml.cpp
    yaml-cpp/src/scanner.cpp
    yaml-cpp/src/scanscalar.cpp
    yaml-cpp/src/scantag.cpp
    yaml-cpp/src/scantoken.cpp
    yaml-cpp/src/simplekey.cpp
    yaml-cpp/src/singledocparser.cpp
    yaml-cpp/src/stream.cpp
    yaml-cpp/src/tag.cpp
    yaml-cpp/src/depthguard.cpp
)

target_include_directories(yamlcpp PRIVATE
    yaml-cpp/include
)

target_include_directories(yamlcpp PRIVATE
    libusb/libusb
    config
)
