#include "Platform.h"
#include <unistd.h>

namespace Platform
{
    std::string ExecutablePath()
    {
        char buf[4096];
        ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
        if (len == -1)
            return {};

        buf[len] = '\0';
        return buf;
    }
}
