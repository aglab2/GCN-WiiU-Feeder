#include "Win.h"

#include <Windows.h>

namespace Win
{
    std::wstring ExecutablePath()
    {
        int count = 128;
        std::wstring path;
        do
        {
            count *= 2;
            path = std::wstring(count, 0);
        }
        while (0 == GetModuleFileName(NULL, path.data(), count));

        return path;
    }
}
