#ifndef GENODE_SYSTEM_TYPES_HPP
#define GENODE_SYSTEM_TYPES_HPP

#include <cstdint>

namespace Gx
{
    // 8 bits integer types
    typedef int8_t  Int8;
    typedef uint8_t Uint8;

    // 16 bits integer types
    typedef int16_t  Int16;
    typedef uint16_t Uint16;

    // 32 bits integer types
    typedef int32_t  Int32;
    typedef uint32_t Uint32;

    // 64 bits integer types
#if defined(_MSC_VER)
    typedef signed   __int64 Int64;
    typedef unsigned __int64 Uint64;
#else
    typedef int64_t  Int64;
    typedef uint64_t Uint64;
#endif

}

#endif