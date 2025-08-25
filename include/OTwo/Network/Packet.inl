#pragma once

#include <magic_enum/magic_enum.hpp>
#include <cstring>

template<typename T>
std::enable_if_t<std::is_enum_v<T>, Packet&>
Packet::operator>>(T& data)
{
    std::underlying_type_t<T> value{};
    *this >> value;

    data = static_cast<T>(value);
    return *this;
}

template<typename T>
std::enable_if_t<Packet::IsSerializable<T>::value && !std::is_same_v<std::decay_t<T>, sf::String>, Packet&>
Packet::operator>>(T& data)
{
    boost::pfr::for_each_field(data, [this] (auto& value)
    {
       auto _ = *this >> value;
    });

    return *this;
}

template<typename T>
std::enable_if_t<std::is_enum_v<T>, Packet&>
Packet::operator<<(const T& data)
{
    *this << magic_enum::enum_integer(data);
    return *this;
}

template<typename T>
std::enable_if_t<Packet::IsSerializable<T>::value && !std::is_same_v<std::decay_t<T>, sf::String>, Packet&>
Packet::operator<<(const T& data)
{
    boost::pfr::for_each_field(data, [this] (const auto& value)
    {
        *this << value;
    });

    return *this;
}

template<typename T>
T Packet::Transform(T value) const
{
    if (Gx::Endian::Native == m_endianness)
        return value;

    if constexpr (sizeof(T) == 1)
    {
        return value;
    }
    else if constexpr (sizeof(T) == 2)
    {
        std::uint16_t temp;
        std::memcpy(&temp, &value, sizeof(T));
        temp = (temp & 0xFF00) >> 8 | (temp & 0x00FF) << 8;
        std::memcpy(&value, &temp, sizeof(T));
        return value;
    }
    else if constexpr (sizeof(T) == 4)
    {
        std::uint32_t temp;
        std::memcpy(&temp, &value, sizeof(T));
        temp = (temp & 0xFF000000) >> 24 |
               (temp & 0x00FF0000) >> 8  |
               (temp & 0x0000FF00) << 8  |
               (temp & 0x000000FF) << 24;
        std::memcpy(&value, &temp, sizeof(T));
        return value;
    }
    else if constexpr (sizeof(T) == 8)
    {
        std::uint64_t temp;
        std::memcpy(&temp, &value, sizeof(T));
        temp = (temp & 0xFF00000000000000ULL) >> 56 |
               (temp & 0x00FF000000000000ULL) >> 40 |
               (temp & 0x0000FF0000000000ULL) >> 24 |
               (temp & 0x000000FF00000000ULL) >> 8  |
               (temp & 0x00000000FF000000ULL) << 8  |
               (temp & 0x0000000000FF0000ULL) << 24 |
               (temp & 0x000000000000FF00ULL) << 40 |
               (temp & 0x00000000000000FFULL) << 56;
        std::memcpy(&value, &temp, sizeof(T));
        return value;
    }

    return value;
}
