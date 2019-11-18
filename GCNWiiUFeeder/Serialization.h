#pragma once

#include <unordered_map>
#include <yaml-cpp/yaml.h>

namespace Serialization
{
    class ISerializable
    {
    public:
        virtual YAML::Node Serialize() const = 0;
    };

    template<typename EnumT>
    class EnumSerializer
    {
    public:
        using NamesMap = std::map<std::string, EnumT>;

        static YAML::Node EncodeBitWise(const NamesMap&, EnumT val);
        static bool DecodeBitWise(const NamesMap&, const YAML::Node& node, EnumT& val);

        static YAML::Node Encode(const NamesMap&, EnumT val);
        static bool Decode(const NamesMap&, const YAML::Node& node, EnumT& val);
    };
};

namespace YAML
{
    template<>
    struct convert<Serialization::ISerializable>
    {
        static Node encode(const Serialization::ISerializable& s);
    };
}
