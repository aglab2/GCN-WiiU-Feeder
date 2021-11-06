#include "GCN.h"
#include "EmuController.h"
#include "Win.h"

#include <iostream>
#include <filesystem>

#include "Mapping.h"

#ifdef _DEBUG
#include "debug.h"
static void test()
{
    Mapping::Debug::test();
}
#endif

static bool Running = true;
BOOL WINAPI CtrlHandler(DWORD event)
{
    if (event == CTRL_CLOSE_EVENT)
    {
        Running = false;
        Sleep(20000);
        return TRUE;
    }
    return FALSE;
}


enum FeederErrors
{
    FE_OK = 0,
    FE_CONFIG_INVALID,
    FE_CONFIG_READ,
    FE_CONFIG_PARSE,
    FE_EMU_LIB,
    FE_USB_LIB,
    FE_ADAPTER,
};

static void die(FeederErrors err, const char* why)
{
    printf("%s\n", why);
    while (0xA != getchar());
    exit(err);
}

int main()
{
#ifdef _DEBUG
    test();
#endif

    std::vector<std::filesystem::path> cfgPaths;
    std::filesystem::path exePath(Win::ExecutablePath());
    auto exeDir = exePath.parent_path();
    for (const auto& entry : std::filesystem::directory_iterator(exeDir))
    {
        if (!entry.is_regular_file())
            continue;

        if (entry.path().extension() == ".yaml")
            cfgPaths.push_back(entry.path());
    }

    if (cfgPaths.empty())
        die(FE_CONFIG_INVALID, "No config files detected!");

    printf("Pick config file\n");
    for (int i = 0; i < cfgPaths.size(); i++)
    {
        auto& cfgPath = cfgPaths[i];
        wprintf(L"%d) %s\n", i + 1, cfgPath.filename().stem().c_str());
    }

    long cfgId = -1;
    do
    {
        printf("Enter config file NUMBER (from 1 to %lld): \n", cfgPaths.size());
        const char cfgIdStr[10] = {};
        scanf_s("%9s", cfgIdStr, (unsigned)_countof(cfgIdStr));

        cfgId = strtol(cfgIdStr, nullptr, 10);
    } 
    while (cfgId <= 0 || cfgId > cfgPaths.size());

    auto cfgPath = cfgPaths[cfgId - 1];

    YAML::Node node;
    try
    {
        node = YAML::LoadFile(cfgPath.u8string());
    }
    catch (...)
    {
        die(FE_CONFIG_READ, "Failed to read config");
    }

    Mapping::Mappers mappers;
    try
    {
        mappers = node.as<Mapping::Mappers>();
    }
    catch (...)
    {
        die(FE_CONFIG_PARSE, "Failed to parse config");
    }

    auto libUsb = Usb::Lib();
    if (!libUsb.Usable())
        die(FE_USB_LIB, "Failed to setup LibUsb");

    auto adapter = GCN::Adapter(libUsb);
    if (!adapter.Usable())
        die(FE_ADAPTER, "Failed to detect WiiU Adapter");

    auto libEmu = Emu::Lib();
    if (!libEmu.Usable())
        die(FE_EMU_LIB, "Failed to allocate ViGEm");

    if (!libEmu.Connect())
        die(FE_EMU_LIB, "Failed to detect ViGEm");

    if (!adapter.Start())
        die(FE_ADAPTER, "Failed to start feeding WiiU inputs");

    SetConsoleCtrlHandler(CtrlHandler, TRUE);

    GCN::Adapter::Control ctl = {};
    ctl.Cmd = 0x11;
    adapter.Write(ctl);

    Emu::Device emuControllers[4] = { Emu::Device(libEmu), Emu::Device(libEmu), Emu::Device(libEmu), Emu::Device(libEmu) };
    char emuControllersPlugged[4] = {};

    printf("Feeder is running!\n");
    GCN::Adapter::Inputs inputs;
    if (!adapter.Read(inputs))
    {
        adapter.Stop();
        return 1;
    }
    
    while (Running)
    {
        for (int i = 0; i <= 3; i++)
        {
            GCN::Controller& controller = inputs.Controllers[i];
            if (emuControllersPlugged[i] != controller.On)
            {
                emuControllersPlugged[i] = controller.On;
                if (controller.On)
                {
                    printf("Controller %d is plugged!\n", i + 1);
                    emuControllers[i].Connect();
                }
                else
                {
                    printf("Controller %d is unplugged!\n", i + 1);
                    emuControllers[i].Disconnect();
                }
            }
        }

        for (int i = 0; i < 3; i++)
        {
            GCN::Controller& controller = inputs.Controllers[i];
            if (!controller.On)
                continue;

            X360::Controller emuControllersInputs = {};
            Mapping::Map(mappers, controller, emuControllersInputs);
            emuControllers[i].Update(emuControllersInputs);
        }

        while (Running)
        {
            if (adapter.ReadInterrupt(inputs))
                break;
        }
    }

    adapter.Stop();

    return 0;
}