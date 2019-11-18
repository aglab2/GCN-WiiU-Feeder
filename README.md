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
