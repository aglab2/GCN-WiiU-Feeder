#pragma once

#include <windows.h>
#include <ViGEm/Client.h>
#include "X360Controller.h"

namespace Emu
{
    class Lib
    {
    public:
        Lib();
        ~Lib();

        bool Connect();

        inline PVIGEM_CLIENT operator*() { return Client; }
        inline bool Usable() { return Client != nullptr; }
        inline bool Connected() { return IsConnected; }
    private:
        PVIGEM_CLIENT Client;
        bool IsConnected;
    };

    class Device
    {
    public:
        Device(Lib& lib);
        ~Device();

        bool Connect();
        bool Disconnect();
        
        bool Update(X360::Controller&);

        inline PVIGEM_TARGET operator*() { return Target; }
        inline bool Usable() { return Target != nullptr; }
        inline bool Connected() { return IsConnected; }
    private:
        Lib& Lib;
        PVIGEM_TARGET Target;
        bool IsConnected;
    };
}