#pragma once

#include "utils.h"

#include <memory>
#include <string>
#include <vector>

#include "ControllerInterface.h"
#include "GCNController.h"
#include "X360Controller.h"

namespace Mapping
{
    class IMapper : public Serialization::ISerializable
    {
    public:
        virtual void Map(const GCN::Controller& from, X360::Controller& to) = 0;
    };
    using IMapperPtr = std::shared_ptr<IMapper>;

    namespace Analog
    {
        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        class LinearMapper final : public IMapper
        {
        public:
            using FromConverter = ControllerInterface::LinearConverter<FromOffsetT, FromStickT>;
            using ToConverter = ControllerInterface::LinearConverter<ToOffsetT, ToStickT>;

            LinearMapper(FromConverter, ToConverter);

            virtual void Map(const GCN::Controller& from, X360::Controller& to);
            virtual YAML::Node Serialize() const;

        private:
            static const std::string name_;
            FromConverter fromConverter_;
            ToConverter toConverter_;
        };

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        class BilinearMapper final : public IMapper
        {
        public:
            using Stretcher = ControllerInterface::BilinearDiagonalStretcher;
            using FromConverter = ControllerInterface::LinearConverter<FromOffsetT, FromStickT>;
            using ToConverter = ControllerInterface::LinearConverter<ToOffsetT, ToStickT>;

            BilinearMapper(FromConverter fX, FromConverter fY, ToConverter tX, ToConverter tY, Stretcher s);

            virtual void Map(const GCN::Controller& from, X360::Controller& to);
            virtual YAML::Node Serialize() const;

        private:
            static const std::string name_;
            const FromConverter fromConverters_[2];
            const ToConverter toConverters_[2];

            const Stretcher stretcher_;
        };

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        using LinearMapperPtr = std::shared_ptr<LinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>>;

        template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
        using BilinearMapperPtr = std::shared_ptr<BilinearMapper<FromOffsetT, FromStickT, ToOffsetT, ToStickT>>;

        using LinearTriggerMapper = LinearMapper<GCN::Axises, unsigned char, X360::Triggers, BYTE>;
        using BilinearTriggerMapper = BilinearMapper<GCN::Axises, unsigned char, X360::Triggers, BYTE>;
        using LinearStickMapper = LinearMapper<GCN::Axises, unsigned char, X360::Thumbs, SHORT>;
        using BilinearStickMapper = BilinearMapper<GCN::Axises, unsigned char, X360::Thumbs, SHORT>;

        using LinearTriggerMapperPtr = std::shared_ptr<LinearTriggerMapper>;
        using BilinearTriggerMapperPtr = std::shared_ptr<BilinearTriggerMapper>;
        using LinearStickMapperPtr = std::shared_ptr<LinearStickMapper>;
        using BilinearStickMapperPtr = std::shared_ptr<BilinearStickMapper>;
    }

    namespace Digital
    {
        class Mapper : public IMapper
        {
        public:
            Mapper(GCN::IEventPtr from, X360::IModifierPtr to);

            virtual void Map(const GCN::Controller& from, X360::Controller& to);
            virtual YAML::Node Serialize() const;

        private:
            GCN::IEventPtr event_;
            X360::IModifierPtr modifier;
        };

        using MapperPtr = std::shared_ptr<Mapper>;
    }

    using Mappers = std::vector<IMapperPtr>;
    void Map(const Mappers&, const GCN::Controller& from, X360::Controller& to);
}

namespace YAML
{
    template<>
    struct convert<Mapping::IMapperPtr>
    {
        static Node encode(const Mapping::IMapperPtr&);
        static bool decode(const Node& node, Mapping::IMapperPtr&);
    };

    template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
    struct convert<Mapping::Analog::LinearMapperPtr<FromOffsetT, FromStickT, ToOffsetT, ToStickT>>
    {
        static bool decode(const Node& node, Mapping::Analog::LinearMapperPtr<FromOffsetT, FromStickT, ToOffsetT, ToStickT>&);
    };

    template<typename FromOffsetT, typename FromStickT, typename ToOffsetT, typename ToStickT>
    struct convert<Mapping::Analog::BilinearMapperPtr<FromOffsetT, FromStickT, ToOffsetT, ToStickT>>
    {
        static bool decode(const Node& node, Mapping::Analog::BilinearMapperPtr<FromOffsetT, FromStickT, ToOffsetT, ToStickT>&);
    };

    template<>
    struct convert<Mapping::Digital::MapperPtr>
    {
        static bool decode(const Node& node, Mapping::Digital::MapperPtr&);
    };
}
