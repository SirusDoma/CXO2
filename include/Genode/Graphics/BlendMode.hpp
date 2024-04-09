#ifndef GENODE_GRAPHICS_BLEND_MODE_HPP
#define GENODE_GRAPHICS_BLEND_MODE_HPP

namespace Gx
{
    enum class BlendMode
    {
        Auto,
        Alpha,
        Additive,
        Multiplicative,
        Min,
        Max,
        None
    };
}

#endif
