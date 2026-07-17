#pragma once

#include <Genode/Network/Packet.hpp>

#include <compact_enc_det/compact_enc_det.h>

// IMPORTANT: this need to be inside Gx namespace
namespace Gx
{
    inline Packet& operator>>(Packet& packet, sf::String& data)
    {
        data.clear();
        auto bytes = std::vector<char>{};

        while (packet)
        {
            std::uint8_t character{};
            packet >> character;

            if (character == 0x00)
                break;

            bytes.push_back(character);
        }

        bool reliable = false;
        int consumed = 0;
        const auto encoding = CompactEncDet::DetectEncoding(
            bytes.data(),
            bytes.size(),
            nullptr,
            nullptr,
            nullptr,
            0,
            Language::UNKNOWN_LANGUAGE,
            CompactEncDet::TextCorpusType::QUERY_CORPUS,
            true,
            &consumed,
            &reliable
        );

        switch (encoding)
        {
            case Encoding::ASCII_7BIT:
            case Encoding::UTF8:
            case Encoding::UTF8UTF8:
            {
                data = sf::String::fromUtf8(bytes.begin(), bytes.end());
                break;
            }
            case Encoding::UTF32LE:
            case Encoding::UTF32BE:
            case Encoding::GB18030:
            {
                // TODO: Handle Big Endian
                auto* ptr = reinterpret_cast<std::uint32_t*>(bytes.data());
                data = sf::String::fromUtf32(ptr, ptr + bytes.size() / sizeof(std::uint32_t));

                break;
            }
            default:
            {
                // TODO: Handle Big Endian
                auto* ptr = reinterpret_cast<std::uint16_t*>(bytes.data());
                data = sf::String::fromUtf16(ptr, ptr + bytes.size() / sizeof(std::uint16_t));

                break;
            }
        }

        return packet;
    }

    inline Packet& operator<<(Packet& packet, const sf::String& data)
    {
        if (data.getSize() > 0)
        {
            int bandwith = 0;
            for (const std::uint32_t c : data)
            {
                if (c <= std::numeric_limits<std::uint8_t>::max())
                    bandwith = bandwith < sizeof(std::uint8_t)  ? sizeof(std::uint8_t)  : bandwith;
                else if (c <= std::numeric_limits<std::uint16_t>::max())
                    bandwith = bandwith < sizeof(std::uint16_t) ? sizeof(std::uint16_t) : bandwith;
                else
                {
                    bandwith = bandwith < sizeof(std::uint32_t) ? sizeof(std::uint32_t) : bandwith;
                    break;
                }
            }

            for (const std::uint32_t c : data)
            {
                if (bandwith == sizeof(std::uint8_t))
                    packet << static_cast<std::uint8_t>(c);
                else if (bandwith == sizeof(std::uint16_t))
                    packet << static_cast<std::uint16_t>(c);
                else
                    packet << c;
            }

            if (static_cast<std::uint32_t>(*(data.end() - 1)) != 0x00)
                packet << static_cast<std::uint8_t>(0);
        }

        return packet;
    }
}
