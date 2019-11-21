#include "ControllerInterface.h"

#include <yaml-cpp/yaml.h>

#include "SerializationImpl.h"

namespace ControllerInterface
{
    static inline float slope(float d) { return (1 - d) / d; }
    static inline float unslope(float s) { return 1 / (1 + s); }

    BilinearDiagonalStretcher::BilinearDiagonalStretcher() : BilinearDiagonalStretcher(1, 1, 1) {}

    BilinearDiagonalStretcher::BilinearDiagonalStretcher(float fromDiagonal, float toDiagonal, float dampingPower)
        : fromSlope_(slope(fromDiagonal))
        , toSlope_(slope(toDiagonal))
        , power_(dampingPower) { }

    void BilinearDiagonalStretcher::Stretch(float& x, float& y) const
    {
        if (x == 0 && y == 0)
            return;

        float X = fabs(x);
        float Y = fabs(y);
        if (Y > X)
            std::swap(X, Y);

        float fromAvg = X + fromSlope_ * Y;
        float toAvg = X + toSlope_ * Y;

        float fullMultiplier = fromAvg / toAvg;
        float addition = fullMultiplier - 1;
        float dampening = pow(fromAvg, power_);

        float multiplier = 1 + addition * dampening;

        x *= multiplier;
        y *= multiplier;
    }

    YAML::Node BilinearDiagonalStretcher::Serialize() const
    {
        YAML::Node node;
        node["fromDiagonal"] = unslope(fromSlope_);
        node["toDiagonal"] = unslope(toSlope_);
        node["power"] = power_;
        return node;
    }
}

namespace YAML
{
    const std::map<std::string, ControllerInterface::AxisComparerType> convert<ControllerInterface::AxisComparerType>::names{
        { "More", ControllerInterface::AxisComparerType::More },
        { "Less", ControllerInterface::AxisComparerType::Less },
    };

    Node convert<ControllerInterface::AxisComparerType>::encode(const ControllerInterface::AxisComparerType& t)
    {
        return Serialization::EnumSerializer<ControllerInterface::AxisComparerType>::Encode(names, t);
    }

    bool convert<ControllerInterface::AxisComparerType>::decode(const Node& node, ControllerInterface::AxisComparerType& t)
    {
        return Serialization::EnumSerializer<ControllerInterface::AxisComparerType>::Decode(names, node, t);
    }

    Node convert<ControllerInterface::BilinearDiagonalStretcher>::encode(const ControllerInterface::BilinearDiagonalStretcher& me)
    {
        return me.Serialize();
    }

    bool convert<ControllerInterface::BilinearDiagonalStretcher>::decode(const Node& node, ControllerInterface::BilinearDiagonalStretcher& stretcher)
    {
        if (!node.IsMap())
            return false;

        auto fromSlopeNode = node["fromDiagonal"];
        auto toSlopeNode = node["toDiagonal"];
        auto powerNode = node["power"];

        if (!fromSlopeNode || !toSlopeNode || !powerNode)
            return false;

        auto fromSlope = fromSlopeNode.as<float>();
        auto toSlope = toSlopeNode.as<float>();
        auto power = powerNode.as<float>();

        stretcher = ControllerInterface::BilinearDiagonalStretcher(fromSlope, toSlope, power);
        return true;
    }
}