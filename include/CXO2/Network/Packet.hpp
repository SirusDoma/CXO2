////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2026 Laurent Gomila (laurent@sfml-dev.org)
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

namespace Cx
{
    class Packet
    {
    public:
        struct ForSerializationTag;

        template<class T>
        using IsSerializable = boost::pfr::is_implicitly_reflectable<T, ForSerializationTag>;

        class Prefix
        {
        public:
            Prefix() = default;

            template<typename T>
            static Prefix Of();

            explicit operator bool() const { return m_encode != nullptr; }

            std::size_t GetSize() const { return m_size; }
            std::size_t GetMaxValue() const { return m_max; }

            void Encode(void* buffer, const std::size_t value) const { m_encode(buffer, value); }
            std::size_t Decode(const void* buffer) const { return m_decode(buffer); }

        private:
            using Encoder = void (*)(void*, std::size_t);
            using Decoder = std::size_t (*)(const void*);

            std::size_t m_size{};
            std::size_t m_max{};
            Encoder     m_encode{};
            Decoder     m_decode{};
        };

        Packet() = default;
        Packet(const Packet&) = default;
        virtual ~Packet() = default;

        Packet& operator=(const Packet& right) = default;

        void SetEndianness(Gx::Endian endianness);
        Gx::Endian GetEndianness() const;

        template<typename T>
        std::enable_if_t<std::is_unsigned_v<T> && !std::is_same_v<T, bool>, void>
        UsePrefix();

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
        Packet& operator>>(wchar_t* data);
        Packet& operator>>(std::string& data);
        Packet& operator>>(std::wstring& data);
        Packet& operator>>(sf::String& data);

        template<typename T>
        std::enable_if_t<std::is_enum_v<T>, Packet&>
        operator>>(T& data);

        template<typename T>
        std::enable_if_t<IsSerializable<T>::value && !std::is_same_v<std::decay_t<T>, sf::String>, Packet&>
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
        Packet& operator<<(const wchar_t* data);
        Packet& operator<<(const std::string& data);
        Packet& operator<<(const std::wstring& data);
        Packet& operator<<(const sf::String& data);

        template<typename T>
        std::enable_if_t<std::is_enum_v<T>, Packet&>
        operator<<(const T& data);

        template<typename T>
        std::enable_if_t<IsSerializable<T>::value && !std::is_same_v<std::decay_t<T>, sf::String>, Packet&>
        operator<<(const T& data);

    protected:
        friend class NetworkClient;

        virtual const void* OnSend(std::size_t& size);
        virtual void OnReceive(const void* data, std::size_t size);

    private:
        template<typename T>
        T Transform(T value) const;

        std::vector<std::byte> m_data{};
        std::size_t m_readPos{};
        bool m_isValid{true};
        Gx::Endian m_endianness{Gx::Endian::Little};
        Prefix m_prefix{};
    };

}

#include <CXO2/Network/Packet.inl>
