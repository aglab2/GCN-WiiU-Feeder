#include "GCNController.h"

#include "ControllerInterfaceImpl.h"
#include "SerializationImpl.h"

namespace YAML
{
    const std::map<std::string, GCN::Axises> convert<GCN::Axises>::names
    {
#define ENUMSTR(name) { #name, GCN::Axises::name } 
            ENUMSTR(AnalogX),
            ENUMSTR(AnalogY),
            ENUMSTR(CStickX),
            ENUMSTR(CStickY),
            ENUMSTR(LeftTrigger),
            ENUMSTR(RightTrigger),
#undef ENUMSTR
    };

    const std::map<std::string, GCN::Buttons> convert<GCN::Buttons>::names
    {
#define ENUMSTR(name) { #name, GCN::Buttons::name } 
            ENUMSTR(A),
            ENUMSTR(B),
            ENUMSTR(X),
            ENUMSTR(Y),
            ENUMSTR(DpadUp),
            ENUMSTR(DpadDown),
            ENUMSTR(DpadLeft),
            ENUMSTR(DpadRight),
            ENUMSTR(Start),
            ENUMSTR(Z),
            ENUMSTR(L),
            ENUMSTR(R),
#undef ENUMSTR
    };

    Node convert<GCN::Axises>::encode(const enum GCN::Axises& thumb)
    {
        return Serializer::Encode(names, thumb);
    }

    bool convert<GCN::Axises>::decode(const Node& node, enum GCN::Axises& thumb)
    {
        return Serializer::Decode(names, node, thumb);
    }

    Node convert<GCN::Buttons>::encode(const enum GCN::Buttons& thumb)
    {
        return Serializer::EncodeBitWise(names, thumb);
    }

    bool convert<GCN::Buttons>::decode(const Node& node, enum GCN::Buttons& thumb)
    {
        return Serializer::DecodeBitWise(names, node, thumb);
    }

    Node convert<GCN::IEventPtr>::encode(const GCN::IEventPtr& ptr)
    {
        return ptr->Serialize();
    }

    bool convert<GCN::IEventPtr>::decode(const Node& node, GCN::IEventPtr& ptr)
    {
        if (node.IsScalar() || node.IsSequence())
        {
            // Button case
            auto buttons = node.as<GCN::Buttons>();
            ptr = std::make_shared<GCN::Button>(buttons);
            return true;
        }
        else if (node.IsMap())
        {
            auto typeNode = node["type"];
            if (!typeNode)
                return false;

            auto type = typeNode.as<std::string>();
            if (type == "axis")
            {
                auto offsetNode = node["offset"];
                auto valueNode = node["axis"];
                auto comparNode = node["comparer"];
                if (!offsetNode || !valueNode || !comparNode)
                    return false;

                auto axises = offsetNode.as<GCN::Axises>();
                auto compar = comparNode.as<ControllerInterface::AxisComparerType>();
                auto value = (unsigned char) valueNode.as<int>();

                ptr = std::make_shared<GCN::Axis>(axises, compar, value);
                return true;
            }
            else
            {
                return false;
            }
        }
        return false;
    }
}

namespace GCN
{
    Button::Button(Buttons b) : IButton(b) { }

    bool Button::Happened(const Controller& c) const
    {
        return Applied(c.Buttons);
    }

    Axis::Axis(Axises a, ControllerInterface::AxisComparerType compar, unsigned char value) 
        : IAxis(value, compar, a) { }

    bool Axis::Happened(const Controller& c) const
    {
        return Applied(&c);
    }
}