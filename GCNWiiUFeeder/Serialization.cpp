#include "Serialization.h"

namespace YAML
{
    Node convert<Serialization::ISerializable>::encode(const Serialization::ISerializable& s)
    {
        return s.Serialize();
    }
}
