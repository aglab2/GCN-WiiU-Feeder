#include "EmuController.h"

namespace Emu
{
    Lib::Lib() 
        : Client(vigem_alloc())
        , IsConnected(false)
    { }

    Lib::~Lib()
    {
        if (Client)
        {
            if (IsConnected)
                vigem_disconnect(Client);

            vigem_free(Client);
        }
    }

    bool Lib::Connect()
    {
        IsConnected = VIGEM_SUCCESS(vigem_connect(Client));
        return IsConnected;
    }

    Device::Device(class Lib& lib)
        : Lib(lib)
        , Target(vigem_target_x360_alloc())
        , IsConnected(false)
    { }

    Device::~Device()
    {
        if (Target)
        {
            Disconnect();
            vigem_target_free(Target);
        }
    }

    bool Device::Connect()
    {
        if (IsConnected)
            return true;

        IsConnected = VIGEM_SUCCESS(vigem_target_add(*Lib, Target));
        return IsConnected;
    }

    bool Device::Disconnect()
    {
        if (!IsConnected)
            return true;

        vigem_target_remove(*Lib, Target);
        IsConnected = false;
        return IsConnected;
    }

    bool Device::Update(X360::Controller& c)
    {
        return VIGEM_SUCCESS(vigem_target_x360_update(*Lib, Target, c));
    }
}