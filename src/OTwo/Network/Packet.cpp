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

#include <OTwo/Network/Packet.hpp>

#include <Genode/System/Exception.hpp>

#include <algorithm>
#include <codecvt>
#include <locale>

void Packet::SetEndianness(const Gx::Endian endianness)
{
    m_endianness = endianness;
}

Gx::Endian Packet::GetEndianness() const
{
    return m_endianness;
}

void Packet::Append(const void* data, const std::size_t sizeInBytes)
{
    if (data && sizeInBytes > 0)
    {
        // ReSharper disable once CppReinterpretCastFromVoidPtr
        const auto* begin = reinterpret_cast<const std::byte*>(data);
        const auto* end   = begin + sizeInBytes;

        m_data.insert(m_data.end(), begin, end);
    }
}

std::size_t Packet::GetReadPosition() const
{
    return m_readPos;
}

void Packet::Clear()
{
    m_data.clear();
    m_readPos = 0;
    m_isValid = true;
}

const void* Packet::GetData() const
{
    return !m_data.empty() ? &m_data[0] : nullptr;
}

std::size_t Packet::GetDataSize() const
{
    return m_data.size();
}

bool Packet::EndOfPacket() const
{
    return m_readPos >= m_data.size();
}

Packet::operator bool() const
{
    return m_isValid;
}

Packet& Packet::operator>>(bool& data)
{
    std::uint8_t value{};
    if (*this >> value)
        data = (value != 0);

    return *this;
}

Packet& Packet::operator>>(std::int8_t& data)
{
    Read(&data, sizeof(data));
    return *this;
}

Packet& Packet::operator>>(std::uint8_t& data)
{
    Read(&data, sizeof(data));
    return *this;
}

Packet& Packet::operator>>(std::int16_t& data)
{
    Read(&data, sizeof(data));
    data = Transform(data);
    return *this;
}

Packet& Packet::operator>>(std::uint16_t& data)
{
    Read(&data, sizeof(data));
    data = Transform(data);
    return *this;
}

Packet& Packet::operator>>(std::int32_t& data)
{
    Read(&data, sizeof(data));
    data = Transform(data);
    return *this;
}

Packet& Packet::operator>>(std::uint32_t& data)
{
    Read(&data, sizeof(data));
    data = Transform(data);
    return *this;
}

Packet& Packet::operator>>(std::int64_t& data)
{
    Read(&data, sizeof(data));
    data = Transform(data);
    return *this;
}

Packet& Packet::operator>>(std::uint64_t& data)
{
    Read(&data, sizeof(data));
    data = Transform(data);
    return *this;
}

Packet& Packet::operator>>(float& data)
{
    Read(&data, sizeof(data));
    data = Transform(data);
    return *this;
}

Packet& Packet::operator>>(double& data)
{
    Read(&data, sizeof(data));
    data = Transform(data);
    return *this;
}

Packet& Packet::operator>>(char* data)
{
    if (!data)
        throw Gx::ArgumentOutOfRangeException("Packet::operator>> Data must not be null");

    while (m_isValid)
    {
        std::uint8_t character{};
        *this >> character;

        *data++ = character;
        if (character == 0x00)
            break;
    }

    return *this;
}

Packet& Packet::operator>>(std::string& data)
{
    data.clear();
    while (m_isValid)
    {
        std::uint8_t character{};
        *this >> character;

        if (character == 0x00)
            break;

        data += static_cast<char>(character);
    }

    return *this;
}

Packet& Packet::operator>>(wchar_t* data)
{
    while (m_isValid)
    {
        std::uint32_t character{};
        *this >> character;

        *data++ = static_cast<wchar_t>(character);
        if (character == 0x00)
            break;
    }

    return *this;
}

Packet& Packet::operator>>(std::wstring& data)
{
    data.clear();
    while (m_isValid)
    {
        std::uint16_t character{};
        *this >> character;

        if (character == 0x00)
            break;

        data += static_cast<wchar_t>(character);
    }

    return *this;
}

Packet& Packet::operator>>(sf::String& data)
{
    data.clear();
    while (m_isValid)
    {
        std::uint16_t character{};
        *this >> character;

        if (character == 0x00)
            break;

        data += static_cast<wchar_t>(character);
    }

    return *this;
}

Packet& Packet::operator<<(const bool data)
{
    *this << static_cast<std::uint8_t>(data);
    return *this;
}

Packet& Packet::operator<<(const std::int8_t data)
{
    Append(&data, sizeof(data));
    return *this;
}

Packet& Packet::operator<<(const std::uint8_t data)
{
    Append(&data, sizeof(data));
    return *this;
}

Packet& Packet::operator<<(const std::int16_t data)
{
    const std::int16_t toWrite = Transform(data);
    Append(&toWrite, sizeof(toWrite));
    return *this;
}

Packet& Packet::operator<<(const std::uint16_t data)
{
    const std::uint16_t toWrite = Transform(data);
    Append(&toWrite, sizeof(toWrite));
    return *this;
}

Packet& Packet::operator<<(const std::int32_t data)
{
    const std::int32_t toWrite = Transform(data);
    Append(&toWrite, sizeof(toWrite));
    return *this;
}

Packet& Packet::operator<<(const std::uint32_t data)
{
    const std::uint32_t toWrite = Transform(data);
    Append(&toWrite, sizeof(toWrite));
    return *this;
}

Packet& Packet::operator<<(const std::int64_t data)
{
    const std::int64_t toWrite = Transform(data);
    Append(&toWrite, sizeof(toWrite));
    return *this;
}

Packet& Packet::operator<<(const std::uint64_t data)
{
    const std::uint64_t toWrite = Transform(data);
    Append(&toWrite, sizeof(toWrite));
    return *this;
}

Packet& Packet::operator<<(const float data)
{
    const float toWrite = Transform(data);
    Append(&toWrite, sizeof(toWrite));
    return *this;
}

Packet& Packet::operator<<(const double data)
{
    const double toWrite = Transform(data);
    Append(&toWrite, sizeof(toWrite));
    return *this;
}

Packet& Packet::operator<<(const char* data)
{
    if (data)
        *this << std::string(data);

    return *this;
}

Packet& Packet::operator<<(const std::string& data)
{
    if (data.size() > 0)
    {
        Append(data.c_str(), data.size());
        if (data.back() != 0x00)
            *this << static_cast<std::uint8_t>(0);
    }

    return *this;
}

Packet& Packet::operator<<(const wchar_t* data)
{
    *this << std::wstring(data ? data : L"");
    return *this;
}

Packet& Packet::operator<<(const std::wstring& data)
{
    if (data.size() > 0)
    {
        for (const wchar_t c : data)
            *this << static_cast<std::uint16_t>(c);

        if (static_cast<std::uint16_t>(data.back()) != 0x00)
            *this << static_cast<std::uint16_t>(0);
    }

    return *this;
}

Packet& Packet::operator<<(sf::String& data)
{
    if (data.getSize() > 0)
    {
        for (const wchar_t c : data)
            *this << static_cast<std::uint16_t>(c);

        if (static_cast<std::uint16_t>(*(data.end() - 1)) != 0x00)
            *this << static_cast<std::uint16_t>(0);
    }

    return *this;
}

const void* Packet::OnSend(std::size_t& size)
{
    size = GetDataSize();
    return GetData();
}

void Packet::OnReceive(const void* data, const std::size_t size)
{
    Append(data, size);
}

void Packet::Read(void* data, const std::size_t size)
{
    if (m_readPos + size <= m_data.size())
    {
        std::memcpy(data, &m_data[m_readPos], size);
        m_readPos += size;
    }
    else
    {
        m_isValid = false;
    }
}