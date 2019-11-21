#include "Mapping.h"

#include <unordered_set>

#include "ControllerInterfaceImpl.h"
#include "MappingImpl.h"

namespace Mapping
{
    namespace Digital
    {
        Mapper::Mapper(GCN::IEventPtr from, X360::IModifierPtr to) : event_(from), modifier(to) { }

        void Mapper::Map(const GCN::Controller& from, X360::Controller& to)
        {
            if (event_->Happened(from))
                modifier->Alter(to);
        }

        YAML::Node Mapper::Serialize() const
        {
            YAML::Node node;
            node["from"] = event_;
            node["to"] = modifier;
            return node;
        }
    }

    void Map(const Mappers& mappers, const GCN::Controller& from, X360::Controller& to)
    {
        for (auto& mapper : mappers)
            mapper->Map(from, to);
    }
}

namespace YAML
{
    Node convert<Mapping::IMapperPtr>::encode(const Mapping::IMapperPtr& mapper)
    {
        return mapper->Serialize();
    }

    bool convert<Mapping::IMapperPtr>::decode(const Node& node, Mapping::IMapperPtr& mapper)
    {
        if (!node.IsMap())
            return false;

        auto typeNode = node["type"];
        if (!typeNode)
        {
            // This is button mapping that does not have the type
            mapper = node.as<Mapping::Digital::MapperPtr>();
            return true;
        }
        else
        {
            auto type = typeNode.as<std::string>();
            if (type == "bilinear")
            {
                mapper = node.as<Mapping::Analog::BilinearStickMapperPtr>();
                return true;
            }
            else if (type == "linear")
            {
                auto toNode = node["to"];
                if (!toNode || !toNode.IsMap())
                    return false;

                auto toOffsetNode = toNode["offset"];
                if (!toOffsetNode || !toOffsetNode.IsScalar())
                    return false;

                auto toOffset = toOffsetNode.as<std::string>();
                if (convert<X360::Thumbs>::is(toOffset))
                {
                    mapper = node.as<Mapping::Analog::LinearStickMapperPtr>();
                    return true;
                }
                else if (convert<X360::Triggers>::is(toOffset))
                {
                    mapper = node.as<Mapping::Analog::LinearTriggerMapperPtr>();
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
        return false;
    }

    bool convert<Mapping::Digital::MapperPtr>::decode(const Node& node, Mapping::Digital::MapperPtr& mapper)
    {
        // TODO: Impl direct simple button mapping
        if (!node.IsMap())
            return false;

        auto fromNode = node["from"];
        auto toNode = node["to"];

        if (!fromNode || !toNode)
            return false;

        auto event = fromNode.as<GCN::IEventPtr>();
        auto modifier = toNode.as<X360::IModifierPtr>();

        mapper = std::make_shared<Mapping::Digital::Mapper>(event, modifier);
        return true;
    }
}
