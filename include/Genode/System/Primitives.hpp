#ifndef GENODE_SYSTEM_TYPES_HPP
#define GENODE_SYSTEM_TYPES_HPP

#include <cstdint>

namespace Gx
{
    // 8 bits integer types
    typedef std::int8_t  Int8;
    typedef std::uint8_t Uint8;

    // 16 bits integer types
    typedef std::int16_t  Int16;
    typedef std::uint16_t Uint16;

    // 32 bits integer types
    typedef std::int32_t  Int32;
    typedef std::uint32_t Uint32;

    // 64 bits integer types
#if defined(_MSC_VER)
    typedef signed   __int64 Int64;
    typedef unsigned __int64 Uint64;
#else
    typedef std::int64_t  Int64;
    typedef std::uint64_t Uint64;
#endif

}

#endif