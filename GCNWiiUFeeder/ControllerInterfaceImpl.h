#pragma once

#include "ControllerInterface.h"
#include "SerializationImpl.h"

#include <algorithm>

namespace ControllerInterface
{
    template<typename T>
    AxisComparer<T>::AxisComparer(Type t) : type_(t) { }

    template<typename T>
    YAML::Node AxisComparer<T>::Serialize() const
    {
        return YAML::Node(type_);
    }

    template<typename T>
    bool AxisComparer<T>::operator()(const T a, const T b) const
    {
        switch (type_)
        {
        case More:
            return a > b;
        case Less:
            return a < b;
        }

        return false;
    }

    template<typename ButtonT, typename HolderT>
    Button<ButtonT, HolderT>::Button(ButtonT button) : button_(button) { }

    template<typename ButtonT, typename HolderT>
    YAML::Node Button<ButtonT, HolderT>::Serialize() const
    {
        return YAML::Node(button_);
    }

    template<typename ButtonT, typename HolderT>
    void Button<ButtonT, HolderT>::Apply(HolderT& controller) const
    {
        controller |= button_;
    }

    template<typename ButtonT, typename HolderT>
    bool Button<ButtonT, HolderT>::Applied(const HolderT& controller) const
    {
        return controller & button_;
    }

    template<typename AxisT, typename OffsetT>
    Axis<AxisT, OffsetT>::Axis(AxisT axis, OffsetT offset) : axis_(axis), offset_(offset) { }

    template<typename AxisT, typename OffsetT>
    YAML::Node Axis<AxisT, OffsetT>::Serialize() const
    {
        YAML::Node node;
        node["type"] = name_;
        node["axis"] = +axis_;
        node["offset"] = offset_;

        return node;
    }

    template<typename AxisT, typename OffsetT>
    const char Axis<AxisT, OffsetT>::name_[] = "axis";

    template<typename AxisT, typename OffsetT>
    void Axis<AxisT, OffsetT>::Apply(void* ptr) const
    {
        auto axis = fieldin(ptr, AxisT, offset_);
        (*axis) = axis_;
    }

    template<typename AxisT, typename OffsetT>
    AxisEvent<AxisT, OffsetT>::AxisEvent(AxisT axis, AxisComparerType compar, OffsetT offset) : axis_(axis), offset_(offset), comparer_(compar) { }

    template<typename AxisT, typename OffsetT>
    YAML::Node AxisEvent<AxisT, OffsetT>::Serialize() const
    {
        YAML::Node node;
        node["type"] = name_;
        node["axis"] = +axis_;
        node["offset"] = offset_;
        node["comparer"] = comparer_;

        return node;
    }

    template<typename AxisT, typename OffsetT>
    bool AxisEvent<AxisT, OffsetT>::Applied(const void* ptr) const
    {
        auto axis = fieldin(ptr, const AxisT, offset_);
        return comparer_(*axis, axis_);
    }

    template<typename AxisT, typename OffsetT>
    const char AxisEvent<AxisT, OffsetT>::name_[] = "axis";

    template<typename OffsetT, typename StickT>
    LinearConverter<OffsetT, StickT>::LinearConverter() : LinearConverter((OffsetT)0, 0, 0) { }

    template<typename OffsetT, typename StickT>
    LinearConverter<OffsetT, StickT>::LinearConverter(OffsetT offset, std::optional<StickT> center, StickT max)
        : offset_(offset), center_(std::move(center)), maxval_(max) { }

    template<typename OffsetT, typename StickT>
    float LinearConverter<OffsetT, StickT>::Convert(StickT value) const
    {
        if (!center_)
            center_ = value;

        return std::clamp((float)(value - *center_) / (float)maxval_, -1.f, 1.f);
    }

    template<typename OffsetT, typename StickT>
    StickT LinearConverter<OffsetT, StickT>::Convert(float value) const
    {
        if (!center_)
            center_ = value;

        return (StickT)((value + *center_) * maxval_);
    }

    template<typename OffsetT, typename StickT>
    YAML::Node LinearConverter<OffsetT, StickT>::Serialize() const
    {
        YAML::Node node;
        if (center_)
            node["center"] = +(*center_);
    
        node["max"] = +maxval_;
        node["offset"] = offset_;
        return node;
    }
}

namespace YAML
{
    template<typename T>
    bool convert<ControllerInterface::AxisComparer<T>>::decode(const Node& node, ControllerInterface::AxisComparer<T>& val)
    {
        auto type = node.as<ControllerInterface::AxisComparerType>();
        val = ControllerInterface::AxisComparer<T>(type);
    }

    template<typename T>
    Node convert<ControllerInterface::AxisComparer<T>>::encode(const ControllerInterface::AxisComparer<T>& val)
    {
        return val.Serialize();
    }

    template<typename T>
    Node convert<ControllerInterface::IEventPtr<T>>::encode(const ControllerInterface::IEventPtr<T>& me)
    {
        return me->Serialize();
    }

    template<typename T>
    Node convert<ControllerInterface::IModifierPtr<T>>::encode(const ControllerInterface::IModifierPtr<T>& me)
    {
        return me->Serialize();
    }

    template<typename T, typename S>
    Node convert<ControllerInterface::LinearConverter<T, S>>::encode(const ControllerInterface::LinearConverter<T, S>& me)
    {
        return me.Serialize();
    }

    template<typename OffsetT, typename StickT>
    bool convert<ControllerInterface::LinearConverter<OffsetT, StickT>>::decode(const Node& node, ControllerInterface::LinearConverter<OffsetT, StickT>& conv)
    {
        if (!node.IsMap())
            return false;

        auto centerNode = node["center"];
        auto maxNode = node["max"];
        auto offsetNode = node["offset"];

        if (!maxNode || !offsetNode)
            return false;

        std::optional<StickT> center;
        if (centerNode)
            center = (StickT)centerNode.as<int>();

        auto maxval = (StickT) maxNode.as<int>();
        auto offset = offsetNode.as<OffsetT>();

        conv = ControllerInterface::LinearConverter<OffsetT, StickT>(offset, center, maxval);
        return true;
    }
}