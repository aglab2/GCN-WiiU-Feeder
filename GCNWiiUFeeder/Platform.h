#pragma once

#include <string>

namespace Platform
{
#ifdef _WIN32
    std::wstring ExecutablePath();
#else
    std::string ExecutablePath();
#endif
}