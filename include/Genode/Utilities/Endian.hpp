#pragma once

namespace Gx
{
    enum class Endian
    {
    #if defined(_MSC_VER) && !defined(__clang__)
        Little = 0,
        Big    = 1,
        Native = Little
    #else
        Little = __ORDER_LITTLE_ENDIAN__,
        Big    = __ORDER_BIG_ENDIAN__,
        Native = __BYTE_ORDER__
    #endif
    };
}