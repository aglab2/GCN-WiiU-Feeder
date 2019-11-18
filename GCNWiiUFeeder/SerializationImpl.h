#pragma once

#include "Serialization.h"

namespace Serialization
{
    template<typename EnumT>
    YAML::Node EnumSerializer<EnumT>::Encode(const NamesMap& m, EnumT val)
    {
        for (auto& [name, v] : m)
            if ((int)v == (int)val)
                return YAML::Node(name);

        return YAML::Node();
    }

    template<typename EnumT>
    bool EnumSerializer<EnumT>::Decode(const NamesMap& m, const YAML::Node& node, EnumT& val)
    {
        if (!node.IsScalar())
            return false;

        auto name = node.as<std::string>();
        auto it = m.find(name);
        if (it == m.end())
            return false;

        val = it->second;
        return true;
    }

    template<typename EnumT>
    YAML::Node EnumSerializer<EnumT>::EncodeBitWise(const NamesMap& m, EnumT val)
    {
        int bitcount = 0;
        for (auto& [name, v] : m)
            // FIXME: dirty
            if ((int)v & (int)val)
                bitcount++;

        if (bitcount == 0)
            return YAML::Node();

        if (bitcount == 1)
            return Encode(m, val);

        YAML::Node node;

        for (auto& [name, v] : m)
        {
            if (!((int)v & (int)val))
                continue;

            node.push_back(name);
        }

        return node;
    }

    template<typename EnumT>
    bool EnumSerializer<EnumT>::DecodeBitWise(const NamesMap& m, const YAML::Node& node, EnumT& val)
    {
        if (node.IsScalar())
        {
            return Decode(m, node, val);
        }
        if (node.IsSequence())
        {
            val = (EnumT)0;
            for (YAML::const_iterator it = node.begin(); it != node.end(); ++it)
            {
                auto name = it->as<std::string>();
                val = (EnumT) (val | m.at(name));
            }

            return val != (EnumT)0;
        }
        return false;
    }
};