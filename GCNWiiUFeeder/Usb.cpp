#include "Usb.h"

namespace Usb
{
    Lib::Lib()
    {
        libusb_init(&Context);
    }

    Lib::~Lib()
    {
        if (Context)
            libusb_exit(Context);
    }

    Device::Device(Lib& lib, uint16_t vendor_id, uint16_t product_id) 
    : DeviceHandle(libusb_open_device_with_vid_pid(*lib, vendor_id, product_id)) 
    , WriteEndpoint(-1)
    , ReadEndpoint(-1)
    , IsInterfaceAcquired(false)
    , Interface(-1)
    {
    }

    Device::~Device()
    {
        ReleaseInterface();

        if (DeviceHandle)
            libusb_close(DeviceHandle);
    }

    bool Device::ClaimInterface(int intf)
    {
        if (IsInterfaceAcquired)
            return false;

        int err = libusb_claim_interface(DeviceHandle, intf);
        if (!err)
            IsInterfaceAcquired = true;

        return !err;
    }

    bool Device::ReleaseInterface()
    {
        if (!IsInterfaceAcquired)
            return false;

        libusb_release_interface(DeviceHandle, Interface);
        IsInterfaceAcquired = false;
        return !IsInterfaceAcquired;
    }

    void Device::SetEndpoints(unsigned char read, unsigned char write)
    {
        ReadEndpoint = read | LIBUSB_ENDPOINT_IN;
        WriteEndpoint = write | LIBUSB_ENDPOINT_OUT;
    }

    bool Device::Write(unsigned char* data, int length)
    {
        int actual;
        int err = libusb_bulk_transfer(DeviceHandle, WriteEndpoint, data, length, &actual, 0);
        return !err && length == actual;
    }

    bool Device::Read(unsigned char* data, int length)
    {
        int actual;
        int err = libusb_bulk_transfer(DeviceHandle, ReadEndpoint, data, length, &actual, 0);
        return !err && length == actual;
    }

    bool Device::ReadInterrupt(unsigned char* data, int length, int timeoutMs)
    {
        int actual;
        int err = libusb_interrupt_transfer(DeviceHandle, ReadEndpoint, data, length, &actual, timeoutMs);
        return !err && length == actual;
    }
}