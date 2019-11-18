#pragma once

#include "Mapping.h"

#include "SerializationImpl.h"

namespace Mapping
{
    namespace Analog
    {
        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        LinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::LinearMapper(FromConverter from, ToConverter to) : fromConverter_(from), toConverter_(to) { }

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        void LinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::Map(const GCN::Controller& from, X360::Controller& to)
        {
            auto fromValPtr = fromConverter_.Get(&from);
            auto toValPtr = toConverter_.Get(&to);

            auto fromValConv = fromConverter_.Convert(*fromValPtr);
            auto toValConv = toConverter_.Convert(fromValConv);

            *toValPtr = toValConv;
        }

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        const std::string LinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::name_ = "linear";

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        YAML::Node LinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::Serialize() const
        {
            YAML::Node node;
            node["type"] = name_;
            node["from"] = fromConverter_;
            node["to"] = toConverter_;
            return node;
        }

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        void BilinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::Map(const GCN::Controller& from, X360::Controller& to)
        {
            float tmpValues[2];
            for (int i = 0; i < 2; i++)
            {
                auto& converter = fromConverters_[i];
                auto fromValPtr = converter.Get(&from);
                tmpValues[i] = converter.Convert(*fromValPtr);
            }

            stretcher_.Stretch(tmpValues[0], tmpValues[1]);

            for (int i = 0; i < 2; i++)
            {
                auto& converter = toConverters_[i];
                auto toValPtr = converter.Get(&to);
                *toValPtr = converter.Convert(tmpValues[i]);
            }
        }

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        BilinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::BilinearMapper(FromConverter fX, FromConverter fY, ToConverter tX, ToConverter tY, Stretcher s)
            : fromConverters_{ fX, fY }, toConverters_{ tX, tY }, stretcher_(s) { }

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        const std::string BilinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::name_ = "bilinear";

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        YAML::Node BilinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::Serialize() const
        {
            YAML::Node node;
            node["type"] = name_;
            node["fromX"] = fromConverters_[0];
            node["fromY"] = fromConverters_[1];
            node["toX"] = toConverters_[0];
            node["toY"] = toConverters_[1];
            node["stretcher"] = stretcher_;
            return node;
        }
    }
}

namespace YAML
{
    template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
    bool convert<Mapping::Analog::LinearMapperPtr<FromOffsetT, FromStickT, ToOffsetT, ToStickT>>::decode(const Node& node, Mapping::Analog::LinearMapperPtr<FromOffsetT, FromStickT, ToOffsetT, ToStickT>& mapper)
    {
        if (!node.IsMap())
            return true;

        auto fromNode = node["from"];
        auto toNode = node["to"];

        if (!fromNode || !toNode)
            return false;
        
        auto from = fromNode.as<Mapping::Analog::LinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::FromConverter>();
        auto to = toNode.as<Mapping::Analog::LinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>::ToConverter>();

        mapper = std::make_shared<Mapping::Analog::LinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>>(from, to);
        return true;
    }

    template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
    bool convert<Mapping::Analog::BilinearMapperPtr<FromOffsetT, FromStickT, ToOffsetT, ToStickT>>::decode(const Node& node, Mapping::Analog::BilinearMapperPtr<FromOffsetT, FromStickT, ToOffsetT, ToStickT>& mapper)
    {
        if (!node.IsMap())
            return true;

        auto fromXNode = node["fromX"];
        auto fromYNode = node["fromY"];
        auto toXNode = node["toX"];
        auto toYNode = node["toY"];
        auto stretcherNode = node["stretcher"];

        if (!fromXNode || !fromYNode || !toXNode || !toYNode || !stretcherNode)
            return false;

        auto fromX = fromXNode.as<GCN::LinearConverter>();
        auto fromY = fromYNode.as<GCN::LinearConverter>();
        auto toX = toXNode.as<X360::ThumbsConverter>();
        auto toY = toYNode.as<X360::ThumbsConverter>();
        auto stretcher = stretcherNode.as<ControllerInterface::BilinearDiagonalStretcher>();

        mapper = std::make_shared<Mapping::Analog::BilinearStickMapper>(fromX, fromY, toX, toY, stretcher);
        return true;
    }
}
