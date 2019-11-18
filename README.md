# GCN-WiiU-Feeder
Gamecube WiiU Adapter Feeder for ViGEm driver

This Feeder was inspired by the vJoy implementation by m4sv and uses the same approach except the program was rewritten in C++ and designed to be more flexible.

# Features
* Plug-n-Play controllers, automatic registration
* Diagonal Stretching to emulate N64 controllers + power
* Axis Centering, arbitrary transformation value
* Digital button <-> Analog stick/trigger mapping
* Flexible YAML configuration

# Used Libraries
* libusb 1.0.23
* ViGEm 1.16.28.0
* yaml-cpp 0.6.3

# How to install?
* Install ViGEm Driver https://github.com/ViGEm/ViGEmBus/releases/download/v1.16.112/ViGEmBus_Setup_1.16.115.exe
* Install WiiU Adapter Driver https://github.com/aglab2/libwdi/releases/download/wii-u-1.0/WiiUAdapterInstaller.exe
* Run the Feeder
