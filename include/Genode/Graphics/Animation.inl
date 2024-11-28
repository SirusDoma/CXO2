#pragma once

namespace Gx
{
    template<typename... Args>
    void Animation::AddFrame(const Frame& first, const Args&... args)
    {
        AddFrame(first);
        AddFrame(args...);
    }
}
