#pragma once

namespace Gx
{
    enum class Platform
    {
        Windows,
        macOS,
        Unix
    };

    inline static Platform GetCurrentPlatform()
    {
#ifdef _WIN32
        return Platform::Windows;
#elif __APPLE__
        return Platform::macOS;
#else
        return Platform::Unix;
#endif
    }
}
