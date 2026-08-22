#pragma once

#include <CXO2/UI/Gauge.hpp>

namespace Cx
{
    template<typename ... Args>
    void Gauge::AddAnimationFrame(const Gx::Animation::Frame& first, const Args&... args)
    {
        AddAnimationFrame(first);
        AddAnimationFrame(args...);
    }
}
