#pragma once

#include "GCNController.h"
#include "Usb.h"

namespace GCN
{
    class Adapter : Usb::Device
    {
    public:
#pragma pack(push, 1)
        struct Inputs
        {
            unsigned char _pad[1];
            Controller Controllers[4];
        };

        struct Control
        {
            unsigned char Cmd;
            unsigned char Active[4];
        };
#pragma pack(pop)

        Adapter(Usb::Lib&);

        bool Start();
        bool Stop();
        bool Read(Inputs&);
        bool ReadInterrupt(Inputs&);
        bool Write(Control&);

        inline bool Usable() { return Usb::Device::Usable(); }
        inline bool Started() { return IsStarted; }

    private:
        bool IsStarted;
    };
}