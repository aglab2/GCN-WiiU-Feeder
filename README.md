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

# How to install and run?
* Install ViGEm Driver https://github.com/ViGEm/ViGEmBus/releases/download/setup-v1.17.333/ViGEmBusSetup_x64.msi
* Install WiiU Adapter Driver installer (Run with Admin Rights!) https://github.com/aglab2/libwdi/releases/download/wii-u-1.0/WiiUAdapterInstaller.exe
* Download the feeder GCNWiiUFeeder from https://github.com/aglab2/GCN-WiiU-Feeder/releases/download/1.1/GCNWiiUFeeder-x64.exe
* Get https://github.com/aglab2/GCN-WiiU-Feeder/releases/download/1.1/examples.zip config files and extract them to the same folder as executable file is
* Run the executable with Admin rights and pick normal.yaml config
* If 'Feeder is running!' appeared, the program has successfully started!
* Enjoy and Test!

# Troubleshooting
If program failed to load, it will output the line that you may use to troubleshoot the problem. L=Line, A=Answer.
* L: **No config files detected**
* A: Make sure you have yaml files from examples.zip extract to the same folder as exe file is.
* L: **Failed to read config**
* A: Feeder can't access the config or it is not YAML file. Make sure you launch with Admin rights and use proper config.
* L: **Failed to parse config**
* A: Config is a proper YAML file but Feeder can't parse it. Check the file.
* L: **Failed to detect WiiU Adapter**
* A: Make sure to have WiiU Adapter Driver installed (Run installer Admin Rights!) or use Zadig to replace the WUP-028 driver to WinUSB.
* L: **Failed to detect ViGEm**
* A: Make sure to have ViGEm Driver installed.
* L: **Failed to setup LibUsb**
* L: **Failed to allocate ViGEm**
* A: Report this as an issue on GitHub.
* L: **Failed to start feeding WiiU inputs**
* A: WiiU Adapter is malfunctioning, try reinserting USB device. If it does not help, report this as an issue on GitHub.
