#pragma once

#include <Genode/Utilities/StringHelper.hpp>
#include <SFML/System/String.hpp>
#include <ced/compact_enc_det.h>

class StringTranscoder
{
public:
    static sf::String Transcode(const std::string& text)
    {
        return Transcode(text.c_str(), text.size());
    }

    static sf::String Transcode(const char* text, std::size_t length)
    {
        for (std::size_t i = 0; i < length; i++)
        {
            if (text[i] == '\0')
            {
                length = i;
                break;
            }
        }

        bool reliable = false;
        int bytesConsumed = 0;
        const auto encoding = CompactEncDet::DetectEncoding(
            text,
            length,
            nullptr,
            nullptr,
            nullptr,
            0,
            Language::UNKNOWN_LANGUAGE,
            CompactEncDet::TextCorpusType::QUERY_CORPUS,
            true,
            &bytesConsumed,
            &reliable
        );

        Encoding fromEncoding = Encoding::UTF8;
        switch (encoding)
        {
            case Encoding::ASCII_7BIT:
            case Encoding::UTF8:
            case Encoding::UTF8UTF8:
            {
                fromEncoding = Encoding::UTF8;
                break;
            }
            case Encoding::UTF32LE:
            case Encoding::UTF32BE:
            case Encoding::GB18030:
            {
                // TODO: Handle Big Endian
                fromEncoding = Encoding::UTF32LE;
                break;
            }
            default:
            {
                // TODO: Handle Big Endian
                if (!reliable)
                    fromEncoding = Encoding::UTF8;
                else
                    fromEncoding = Encoding::UTF16LE;

                break;
            }
        }

        switch (fromEncoding)
        {
            case Encoding::UTF8:
            {
                auto* ptr = reinterpret_cast<const std::uint8_t*>(text);
                return Gx::StringHelper::Trim(sf::String::fromUtf8(ptr, ptr + length / sizeof(std::uint8_t)));
            }
            case Encoding::UTF32LE:
            {
                auto* ptr = reinterpret_cast<const std::uint32_t*>(text);
                return Gx::StringHelper::Trim(sf::String::fromUtf16(ptr, ptr + length / sizeof(std::uint32_t)));
            }
            default:
            {
                auto* ptr = reinterpret_cast<const std::uint32_t*>(text);
                return Gx::StringHelper::Trim(sf::String::fromUtf16(ptr, ptr + length / sizeof(std::uint16_t)));
            }
        }
    }

};