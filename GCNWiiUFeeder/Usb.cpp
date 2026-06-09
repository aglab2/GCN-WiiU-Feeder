#include "Usb.h"

#include <stdio.h>
#include <unistd.h>

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

    void Device::ClaimInterface(int intf)
    {
        if (IsInterfaceAcquired)
        {
            printf("Interface already claimed\n");
            return;
        }

        while (!IsInterfaceAcquired)
        {
            int err = libusb_claim_interface(DeviceHandle, intf);
            if (!err)
            {
                IsInterfaceAcquired = true;
                break;
            }

            if (libusb_kernel_driver_active(DeviceHandle, 0) == 1)
            {
                printf("Kernel driver active. Detaching...\n");
                if (libusb_detach_kernel_driver(DeviceHandle, 0) == 0)
                {
                    printf("Kernel driver detached successfully.\n");
                }
            }
            else
            {
                printf("Claiming interface %d failed with error %d, retrying...\n", intf, err);
                sleep(1);
            }
        }
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