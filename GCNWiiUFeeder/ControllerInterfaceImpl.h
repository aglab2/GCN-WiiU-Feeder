#pragma once

#include "ControllerInterface.h"
#include "SerializationImpl.h"

namespace
{
    static const std::map<std::string, ControllerInterface::AxisComparerType> axisComparerNames {
        { "None", ControllerInterface::AxisComparerType::None },
        { "More", ControllerInterface::AxisComparerType::More },
        { "Less", ControllerInterface::AxisComparerType::Less },
    };
}

namespace ControllerInterface
{
    template<typename T>
    AxisComparer<T>::AxisComparer(Type t) : type_(t) { }

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
    template<typename T>
    AxisComparer<T>::operator bool() const
    {
        return type_ != None;
    }

    template<typename T>
    YAML::Node AxisComparer<T>::Serialize() const
    {
        return Serialization::EnumSerializer<AxisComparerType>::Encode(axisComparerNames, type_);
    }

    template<typename ButtonT, typename HolderT>
    ButtonSerializable<ButtonT, HolderT>::ButtonSerializable(ButtonT button) : button_(button) { }

    template<typename ButtonT, typename HolderT>
    YAML::Node ButtonSerializable<ButtonT, HolderT>::Serialize() const
    {
        return YAML::Node(button_);
    }

    template<typename ButtonT, typename HolderT>
    void ButtonSerializable<ButtonT, HolderT>::Apply(HolderT& controller) const
    {
        controller |= button_;
    }

    template<typename ButtonT, typename HolderT>
    bool ButtonSerializable<ButtonT, HolderT>::Applied(const HolderT& controller) const
    {
        return controller & button_;
    }

    template<typename AxisT, typename OffsetT>
    AxisSerializable<AxisT, OffsetT>::AxisSerializable(AxisT axis, AxisComparerType compar, OffsetT offset)
        : axis_(axis), comparer_(compar), offset_(offset) { }

    template<typename AxisT, typename OffsetT>
    YAML::Node AxisSerializable<AxisT, OffsetT>::Serialize() const
    {
        YAML::Node node;
        node["type"] = name_;
        node["axis"] = +axis_;
        node["offset"] = offset_;
        if (comparer_)
            node["comparer"] = comparer_;

        return node;
    }

    template<typename AxisT, typename OffsetT>
    const char AxisSerializable<AxisT, OffsetT>::name_[] = "axis";

    template<typename AxisT, typename OffsetT>
    void AxisSerializable<AxisT, OffsetT>::Apply(void* ptr) const
    {
        auto axis = fieldin(ptr, AxisT, offset_);
        (*axis) = axis_;
    }

    template<typename AxisT, typename OffsetT>
    bool AxisSerializable<AxisT, OffsetT>::Applied(const void* ptr) const
    {
        auto axis = fieldin(ptr, const AxisT, offset_);
        return comparer_(*axis, axis_);
    }

    template<typename OffsetT, typename StickT>
    LinearConverter<OffsetT, StickT>::LinearConverter() : LinearConverter((OffsetT)0, 0, 0) { }

    template<typename OffsetT, typename StickT>
    LinearConverter<OffsetT, StickT>::LinearConverter(OffsetT offset, StickT center, StickT max)
        : offset_(offset), center_(center), maxval_(max) { }

    template<typename OffsetT, typename StickT>
    float LinearConverter<OffsetT, StickT>::Convert(StickT value) const
    {
        return (float)(value - center_) / (float)maxval_;
    }

    template<typename OffsetT, typename StickT>
    StickT LinearConverter<OffsetT, StickT>::Convert(float value) const
    {
        return (StickT)((value + center_) * maxval_);
    }

    template<typename OffsetT, typename StickT>
    YAML::Node LinearConverter<OffsetT, StickT>::Serialize() const
    {
        YAML::Node node;
        node["center"] = +center_;
        node["max"] = +maxval_;
        node["offset"] = offset_;
        return node;
    }
}

namespace YAML
{
    template<typename T>
    Node convert<ControllerInterface::AxisComparer<T>>::encode(const ControllerInterface::AxisComparer<T>& me)
    {
        return me.Serialize();
    }

    template<typename T>
    bool convert<ControllerInterface::AxisComparer<T>>::decode(const Node& node, ControllerInterface::AxisComparer<T>& val)
    {
        if (!node.IsScalar())
            return false;

        auto name = node.as<std::string>();
        return Serialization::EnumSerializer<ControllerInterface::AxisComparerType>::Decode(axisComparerNames, node, val);
    }

    template<typename T>
    Node convert<ControllerInterface::IPressablePtr<T>>::encode(const ControllerInterface::IPressablePtr<T>& me)
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

        if (!centerNode || !maxNode || !offsetNode)
            return false;

        auto center = (StickT) centerNode.as<int>();
        auto maxval = (StickT) maxNode.as<int>();
        auto offset = offsetNode.as<OffsetT>();

        conv = ControllerInterface::LinearConverter<OffsetT, StickT>(offset, center, maxval);
        return true;
    }
}