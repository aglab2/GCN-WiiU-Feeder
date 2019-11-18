#include "GCN.h"

constexpr auto VENDOR_ID = 0x57E;
constexpr auto PRODUCT_ID = 0x337;

namespace GCN
{
    Adapter::Adapter(Usb::Lib& lib)
    : Usb::Device(lib, VENDOR_ID, PRODUCT_ID)
    {
        SetEndpoints(1, 2);
    }

    bool Adapter::Start()
    {
        if (!ClaimInterface(0))
            return false;

        unsigned char data = 19;
        return Usb::Device::Write(&data, 1);
    }

    bool Adapter::Stop()
    {
        return Usb::Device::ReleaseInterface();
    }

    bool Adapter::Read(Inputs& buf)
    {
        return Usb::Device::ReadInterrupt((unsigned char*)&buf, sizeof(Inputs), 100);
    }

    bool Adapter::Write(Control& ctl)
    {
        return Usb::Device::Write((unsigned char*)&ctl, sizeof(Control));
    }
}