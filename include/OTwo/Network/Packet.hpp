////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2024 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#pragma once

#include <Genode/Utilities/Endian.hpp>
#include <SFML/System/String.hpp>

#include <boost/pfr.hpp>
#include <vector>
#include <string>
#include <cstdint>

class Packet
{
public:
    struct ForSerializationTag;

    template<class T>
    using IsSerializable = boost::pfr::is_implicitly_reflectable<T, ForSerializationTag>;

    Packet() = default;
    Packet(const Packet&) = default;
    virtual ~Packet() = default;

    Packet& operator=(const Packet& right) = default;

    void SetEndianness(Gx::Endian endianness);
    Gx::Endian GetEndianness() const;

    void Append(const void* data, std::size_t sizeInBytes);
    void Read(void* data, std::size_t size);

    [[nodiscard]] std::size_t GetReadPosition() const;
    void Clear();

    const void* GetData() const;
    std::size_t GetDataSize() const;
    bool EndOfPacket() const;

    // ReSharper disable once CppNonExplicitConversionOperator
    operator bool() const;

    Packet& operator>>(bool& data);
    Packet& operator>>(std::int8_t& data);
    Packet& operator>>(std::uint8_t& data);
    Packet& operator>>(std::int16_t& data);
    Packet& operator>>(std::uint16_t& data);
    Packet& operator>>(std::int32_t& data);
    Packet& operator>>(std::uint32_t& data);
    Packet& operator>>(std::int64_t& data);
    Packet& operator>>(std::uint64_t& data);
    Packet& operator>>(float& data);
    Packet& operator>>(double& data);
    Packet& operator>>(char* data);
    Packet& operator>>(std::string& data);
    Packet& operator>>(wchar_t* data);
    Packet& operator>>(std::wstring& data);
    Packet& operator>>(sf::String& data);

    template<typename T>
    std::enable_if_t<std::is_enum_v<T>, Packet&>
    operator>>(T& data);

    template<typename T>
    std::enable_if_t<IsSerializable<T>::value, Packet&>
    operator>>(T& data);

    Packet& operator<<(bool data);
    Packet& operator<<(std::int8_t data);
    Packet& operator<<(std::uint8_t data);
    Packet& operator<<(std::int16_t data);
    Packet& operator<<(std::uint16_t data);
    Packet& operator<<(std::int32_t data);
    Packet& operator<<(std::uint32_t data);
    Packet& operator<<(std::int64_t data);
    Packet& operator<<(std::uint64_t data);
    Packet& operator<<(float data);
    Packet& operator<<(double data);
    Packet& operator<<(const char* data);
    Packet& operator<<(const std::string& data);
    Packet& operator<<(const wchar_t* data);
    Packet& operator<<(const std::wstring& data);
    Packet& operator<<(sf::String& data);

    template<typename T>
    std::enable_if_t<std::is_enum_v<T>, Packet&>
    operator<<(const T& data);

    template<typename T>
    std::enable_if_t<IsSerializable<T>::value, Packet&>
    operator<<(const T& data);

protected:
    friend class NetworkAdapter;

    virtual const void* OnSend(std::size_t& size);
    virtual void OnReceive(const void* data, std::size_t size);

private:
    template<typename T>
    T Transform(T value) const;

    std::vector<std::byte> m_data{};
    std::size_t m_readPos{};
    bool m_isValid{true};
    Gx::Endian m_endianness{Gx::Endian::Little};
};

#include <OTwo/Network/Packet.inl>
