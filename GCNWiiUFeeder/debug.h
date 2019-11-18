#pragma once

#include "GCNController.h"
#include "X360Controller.h"

namespace Mapping
{
namespace Debug
{
    void Map(const GCN::Controller& from, X360::Controller& to);

#ifdef _DEBUG
    void test();
#endif
}
}