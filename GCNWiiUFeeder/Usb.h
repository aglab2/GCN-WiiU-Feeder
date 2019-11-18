#pragma once

#pragma warning( push )
#pragma warning( disable : 26451 4200 )
#include <libusb/libusb.h>
#pragma warning( pop )

namespace Usb
{
    class Lib
    {
    public:
        Lib();
        ~Lib();

        inline bool Usable() { return Context != nullptr; }
        inline libusb_context* operator*() { return Context; }

    private:
        libusb_context* Context;
    };

    class Device
    {
    public:
        Device(Lib&, uint16_t vendor_id, uint16_t product_id);
        ~Device();

        inline bool Usable() { return DeviceHandle != nullptr; }
        inline libusb_device_handle* operator*() { return DeviceHandle; }

        bool ClaimInterface(int intf);
        bool ReleaseInterface();

        void SetEndpoints(unsigned char read, unsigned char write);

        bool Write(unsigned char* data, int length);
        bool Read (unsigned char* data, int length);
        bool ReadInterrupt(unsigned char* data, int length, int timeoutMs);

    private:
        libusb_device_handle* DeviceHandle;

        unsigned char WriteEndpoint;
        unsigned char ReadEndpoint;

        bool IsInterfaceAcquired;
        int Interface;
    };
}