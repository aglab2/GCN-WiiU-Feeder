#pragma once

#include <memory>
#include <yaml-cpp/yaml.h>

#include "ControllerInterface.h"

namespace GCN
{
#pragma pack(push, 1)
    struct Controller
    {
        unsigned char On;

        union
        {
            unsigned short Buttons;

            unsigned short A : 1;
            unsigned short B : 1;
            unsigned short X : 1;
            unsigned short Y : 1;
            unsigned short DpadLeft : 1;
            unsigned short DpadRight : 1;
            unsigned short DpadDown : 1;
            unsigned short DpadUp : 1;
            unsigned short Start : 1;
            unsigned short Z : 1;
            unsigned short R : 1;
            unsigned short L : 1;
        };

        unsigned char AnalogX;
        unsigned char AnalogY;
        unsigned char CStickX;
        unsigned char CStickY;
        unsigned char LeftTrigger;
        unsigned char RightTrigger;
    };
#pragma pack(pop)

    using IEvent = ControllerInterface::IEvent<Controller>;
    using IEventPtr = ControllerInterface::IEventPtr<Controller>;

    enum Buttons : unsigned short
    {
        A         = 1 << 0,
        B         = 1 << 1,
        X         = 1 << 2,
        Y         = 1 << 3,
        DpadLeft  = 1 << 4,
        DpadRight = 1 << 5,
        DpadDown  = 1 << 6,
        DpadUp    = 1 << 7,
        Start     = 1 << 8,
        Z         = 1 << 9,
        R         = 1 << 10,
        L         = 1 << 11,
    };

    YAML::Emitter& operator<<(YAML::Emitter&, enum Buttons);

    enum Axises : size_t
    {
        AnalogX      = offsetof(Controller, AnalogX),
        AnalogY      = offsetof(Controller, AnalogY),
        CStickX      = offsetof(Controller, CStickX),
        CStickY      = offsetof(Controller, CStickY),
        LeftTrigger  = offsetof(Controller, LeftTrigger),
        RightTrigger = offsetof(Controller, RightTrigger),
    };

    YAML::Emitter& operator<<(YAML::Emitter&, enum Axises);

    using IButton = ControllerInterface::Button<Buttons, unsigned short>;
    class Button final : public IEvent
                       , public IButton
    {
    public:
        Button(Buttons);

        virtual bool Happened(const Controller&) const;
    };

    using IAxis = ControllerInterface::AxisEvent<unsigned char, Axises>;
    struct Axis final : public IEvent
                      , public IAxis
    {
    public:
        Axis(Axises, ControllerInterface::AxisComparerType type, unsigned char value);

        virtual bool Happened(const Controller&) const;
    };

    using LinearConverter = ControllerInterface::LinearConverter<GCN::Axises, unsigned char>;
}

namespace YAML
{
    template<>
    struct convert<GCN::Axises>
    {
        using Serializer = Serialization::EnumSerializer<GCN::Axises>;

        static const std::map<std::string, GCN::Axises> names;

        static Node encode(const enum GCN::Axises& thumb);
        static bool decode(const Node& node, enum GCN::Axises& thumb);
    };

    template<>
    struct convert<GCN::Buttons>
    {
        using Serializer = Serialization::EnumSerializer<GCN::Buttons>;

        static const std::map<std::string, GCN::Buttons> names;

        static Node encode(const enum GCN::Buttons& thumb);
        static bool decode(const Node& node, enum GCN::Buttons& thumb);
    };

    template<>
    struct convert<GCN::IEventPtr>
    {
        static Node encode(const GCN::IEventPtr&);
        static bool decode(const Node& node, GCN::IEventPtr&);
    };
}