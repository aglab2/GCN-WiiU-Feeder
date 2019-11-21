#include "X360Controller.h"
#include "ControllerInterfaceImpl.h"
#include "SerializationImpl.h"

namespace YAML
{
    const std::map<std::string, X360::Thumbs> convert<X360::Thumbs>::names
    {
#define ENUMSTR(name) { #name, X360::Thumbs::name } 
        ENUMSTR(LeftX),
        ENUMSTR(LeftY),
        ENUMSTR(RightX),
        ENUMSTR(RightY),
#undef ENUMSTR
    };

    const std::map<std::string, X360::Triggers> convert<X360::Triggers>::names
    {
#define ENUMSTR(name) { #name, X360::Triggers::name } 
        ENUMSTR(LeftTrigger),
        ENUMSTR(RightTrigger),
#undef ENUMSTR
    };

    const std::map<std::string, X360::Buttons> convert<X360::Buttons>::names
    {
#define ENUMSTR(name) { #name, X360::Buttons::name } 
        ENUMSTR(DpadUp),
        ENUMSTR(DpadDown),
        ENUMSTR(DpadLeft),
        ENUMSTR(DpadRight),
        ENUMSTR(Start),
        ENUMSTR(Back),
        ENUMSTR(LeftThumb),
        ENUMSTR(RightThumb),
        ENUMSTR(L),
        ENUMSTR(R),
        ENUMSTR(Guide),
        ENUMSTR(A),
        ENUMSTR(B),
        ENUMSTR(X),
        ENUMSTR(Y),
#undef ENUMSTR
    };

    // TODO: Avoid repetition?
    Node convert<X360::Thumbs>::encode(const enum X360::Thumbs& thumb)
    {
        return Serializer::Encode(names, thumb);
    }

    bool convert<X360::Thumbs>::decode(const Node& node, enum X360::Thumbs& thumb)
    {
        return Serializer::Decode(names, node, thumb);
    }

    bool convert<X360::Thumbs>::is(std::string& name)
    {
        return names.find(name) != names.end();
    }

    Node convert<X360::Triggers>::encode(const enum X360::Triggers& thumb)
    {
        return Serializer::Encode(names, thumb);
    }

    bool convert<X360::Triggers>::decode(const Node& node, enum X360::Triggers& thumb)
    {
        return Serializer::Decode(names, node, thumb);
    }

    bool convert<X360::Triggers>::is(std::string& name)
    {
        return names.find(name) != names.end();
    }

    Node convert<X360::Buttons>::encode(const enum X360::Buttons& thumb)
    {
        return Serializer::EncodeBitWise(names, thumb);
    }

    bool convert<X360::Buttons>::decode(const Node& node, enum X360::Buttons& thumb)
    {
        return Serializer::DecodeBitWise(names, node, thumb);
    }

    Node convert<X360::IModifierPtr>::encode(const X360::IModifierPtr& ptr)
    {
        return ptr->Serialize();
    }

    bool convert<X360::IModifierPtr>::decode(const Node& node, X360::IModifierPtr& ptr)
    {
        if (node.IsScalar() || node.IsSequence())
        {
            // Button case
            auto buttons = node.as<X360::Buttons>();
            ptr = std::make_shared<X360::Button>(buttons);
            return true;
        }

        if (node.IsMap())
        {
            auto typeNode = node["type"];
            if (!typeNode)
                return false;

            auto type = typeNode.as<std::string>();
            if (type == "axis")
            {
                auto offsetNode = node["offset"];
                if (!offsetNode)
                    return false;

                auto offsetStr = offsetNode.as<std::string>();
                if (convert<X360::Thumbs>::is(offsetStr))
                {
                    ptr = node.as<X360::ThumbPtr>();
                    return true;
                }
                else if (convert<X360::Triggers>::is(offsetStr))
                {
                    ptr = node.as<X360::TriggerPtr>();
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }

        return false;
    }

    bool convert<X360::ThumbPtr>::decode(const Node& node, X360::ThumbPtr& ptr)
    {
        if (!node.IsMap())
            return false;

        auto offsetNode = node["offset"];
        auto valueNode = node["axis"];

        if (!offsetNode || !valueNode)
            return false;

        auto offset = offsetNode.as<X360::Thumbs>();
        auto value = (SHORT)valueNode.as<int>();

        ptr = std::make_shared<X360::Thumb>(offset, value);
        return true;
    }

    bool convert<X360::TriggerPtr>::decode(const Node& node, X360::TriggerPtr& ptr)
    {
        if (!node.IsMap())
            return false;

        auto offsetNode = node["offset"];
        auto valueNode = node["axis"];

        if (!offsetNode || !valueNode)
            return false;

        auto offset = offsetNode.as<X360::Triggers>();
        auto value = (BYTE)valueNode.as<int>();

        ptr = std::make_shared<X360::Trigger>(offset, value);
        return true;
    }
}

namespace X360
{
    Button::Button(Buttons button) : IButton(button) { }

    void Button::Alter(Controller& c) const
    {
        Apply(c.wButtons);
    }

    template<typename AxisT, typename OffsetT>
    Axis<AxisT, OffsetT>::Axis(IAxis<AxisT, OffsetT> me) : IAxis<AxisT, OffsetT>(me) { }

    template<typename AxisT, typename OffsetT>
    void Axis<AxisT, OffsetT>::Alter(Controller& c) const
    {
        return IAxis<AxisT, OffsetT>::Apply(&c);
    }

    // TODO: Stupid!
    Thumb::Thumb(Thumbs thumb, SHORT value) : Axis(Intf(value,  thumb)) { }

    Trigger::Trigger(Triggers trigger, BYTE value) : Axis(Intf(value, trigger)) { }
}