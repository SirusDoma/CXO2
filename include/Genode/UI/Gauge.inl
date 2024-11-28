#pragma once

#include <Genode/UI/Gauge.hpp>

namespace Gx
{
    template<typename ... Args>
    void Gauge::AddAnimationFrame(const Animation::Frame& first, const Args&... args)
    {
        AddAnimationFrame(first);
        AddAnimationFrame(args...);
    }
}