#pragma once

#include <Genode/Network/Packet.hpp>

#include <magic_enum/magic_enum.hpp>

#include <type_traits>

namespace Gx
{
    template<typename T>
    std::enable_if_t<std::is_enum_v<T>, Packet&>
    operator>>(Packet& packet, T& data)
    {
        std::underlying_type_t<T> value{};
        packet >> value;

        data = static_cast<T>(value);
        return packet;
    }

    template<typename T>
    std::enable_if_t<std::is_enum_v<T>, Packet&>
    operator<<(Packet& packet, const T& data)
    {
        packet << magic_enum::enum_integer(data);
        return packet;
    }
}
