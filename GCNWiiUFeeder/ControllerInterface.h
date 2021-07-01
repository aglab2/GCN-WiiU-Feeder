#pragma once

#include "utils.h"
#include "Serialization.h"

#include <map>
#include <memory>
#include <string>
#include <optional>

namespace YAML
{
    class Node;
}

namespace ControllerInterface
{
    // HolderT is base of ButtonT enum
    template<typename ButtonT, typename HolderT>
    class Button : public virtual Serialization::ISerializable
    {
    public:
        Button(ButtonT button);

        virtual YAML::Node Serialize() const override;

        void Apply  (HolderT& controller) const;
        bool Applied(const HolderT& controller) const;

    private:
        const ButtonT button_;
    };

    enum AxisComparerType
    {
        Less,
        More,
    };

    template<typename T>
    class AxisComparer final : public virtual Serialization::ISerializable
    {
    public:
        using Type = AxisComparerType;
        AxisComparer(Type t);

        virtual YAML::Node Serialize() const override;

        bool operator()(const T a, const T b) const;

    private:
        const Type type_;
    };

    // OffsetT must have size_t as a Base
    template<typename AxisT, typename OffsetT>
    class Axis : public virtual Serialization::ISerializable
    {
    public:
        Axis(AxisT axis, OffsetT offset);
        virtual YAML::Node Serialize() const override;

        void Apply(void* ptr) const;

    private:
        static const char name_[];
        AxisT axis_;
        OffsetT offset_;
    };

    template<typename AxisT, typename OffsetT>
    class AxisEvent : public virtual Serialization::ISerializable
    {
    public:
        AxisEvent(AxisT axis, AxisComparerType compar, OffsetT offset);
        virtual YAML::Node Serialize() const override;

        bool Applied(const void* ptr) const;

    private:
        static const char name_[];
        AxisT axis_;
        OffsetT offset_;
        AxisComparer<AxisT> comparer_;
    };

    template<typename ControllerT>
    class IEvent : public virtual Serialization::ISerializable
    {
    public:
        virtual bool Happened(const ControllerT&) const = 0;
    };

    template<typename ControllerT>
    class IModifier : public virtual Serialization::ISerializable
    {
    public:
        virtual void Alter(ControllerT&) const = 0;
    };

    template<typename ControllerT>
    using IEventPtr = std::shared_ptr<IEvent<ControllerT>>;

    template<typename ControllerT>
    using IModifierPtr = std::shared_ptr<IModifier<ControllerT>>;

    // Converter from anything to [-1; 1] floating point axis
    template<typename OffsetT, typename StickT>
    class LinearConverter final : public Serialization::ISerializable
    {
    public:
        LinearConverter();
        LinearConverter(OffsetT offset, std::optional<StickT> center, StickT max);

        StickT* Get(void* ptr) const { return fieldin(ptr, StickT, offset_); }
        const StickT* Get(const void* ptr) const { return fieldin(ptr, const StickT, offset_); }

        // convert from StickT to [-1; 1]
        float Convert(StickT) const;

        // convert from [-1; 1] to StickT
        StickT Convert(float) const;

        virtual YAML::Node Serialize() const override;

        OffsetT offset_;
        mutable std::optional<StickT> center_;
        StickT maxval_;
    };

    // Symmetric axial stretcher
    // both sticks are assumed to be mapped in [-1; 1]
    class BilinearDiagonalStretcher final : public Serialization::ISerializable
    {
    public:
        BilinearDiagonalStretcher();
        BilinearDiagonalStretcher(float fromDiagonal, float toDiagonal, float dampingPower = 1);

        void Stretch(float&, float&) const;

        virtual YAML::Node Serialize() const override;

        float fromSlope_;
        float toSlope_;
        float power_;
    };
}

namespace YAML
{
    template<>
    struct convert<ControllerInterface::AxisComparerType>
    {
        static const std::map<std::string, ControllerInterface::AxisComparerType> names;

        static Node encode(const ControllerInterface::AxisComparerType&);
        static bool decode(const Node& node, ControllerInterface::AxisComparerType&);
    };

    template<typename T>
    struct convert<ControllerInterface::AxisComparer<T>>
    {
        static Node encode(const ControllerInterface::AxisComparer<T>&);
        static bool decode(const Node& node, ControllerInterface::AxisComparer<T>&);
    };

    template<typename T>
    struct convert<ControllerInterface::IEventPtr<T>>
    {
        static Node encode(const ControllerInterface::IEventPtr<T>&);
    };

    template<typename T>
    struct convert<ControllerInterface::IModifierPtr<T>>
    {
        static Node encode(const ControllerInterface::IModifierPtr<T>&);
    };

    template<typename T, typename S>
    struct convert<ControllerInterface::LinearConverter<T, S>>
    {
        static Node encode(const ControllerInterface::LinearConverter<T, S>&);
        static bool decode(const Node& node, ControllerInterface::LinearConverter<T, S>&);
    };

    template<>
    struct convert<ControllerInterface::BilinearDiagonalStretcher>
    {
        static Node encode(const ControllerInterface::BilinearDiagonalStretcher&);
        static bool decode(const Node& node, ControllerInterface::BilinearDiagonalStretcher&);
    };
}