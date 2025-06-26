#pragma once

#include <SFML/System/String.hpp>

#ifdef USE_LIBICONV
#include <ced/compact_enc_det.h>
#include <libiconv/iconv.h>
#endif

#include <iostream>
#include <regex>

class StringTranscoder
{
public:
    static sf::String Transcode(const std::string& text)
    {
        return Transcode(text.c_str(), text.size());
    }

    static sf::String Transcode(const char* text, std::size_t length)
    {
#if USE_LIBICONV
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
        auto encoding = CompactEncDet::DetectEncoding(
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

        std::string fromEncoding = "UTF-8";
        switch (encoding)
        {
            case Encoding::ISO_8859_1:
                fromEncoding = reliable ? "ISO-8859-1" : "EUC-KR";
                break;
            case Encoding::ISO_8859_2:
                fromEncoding = "ISO-8859-2";
                break;
            case Encoding::ISO_8859_3:
                fromEncoding = "ISO-8859-3";
                break;
            case Encoding::ISO_8859_4:
                fromEncoding = "ISO-8859-4";
                break;
            case Encoding::ISO_8859_5:
                fromEncoding = "ISO-8859-5";
                break;
            case Encoding::ISO_8859_6:
                fromEncoding = "ISO-8859-6";
                break;
            case Encoding::ISO_8859_7:
                fromEncoding = "ISO-8859-7";
                break;
            case Encoding::ISO_8859_8:
                fromEncoding = "ISO-8859-8";
                break;
            case Encoding::ISO_8859_9:
                fromEncoding = "ISO-8859-9";
                break;
            case Encoding::ISO_8859_10:
                fromEncoding = "ISO-8859-10";
                break;
            case Encoding::ISO_8859_13:
                fromEncoding = "ISO-8859-13";
            break;
            case Encoding::JAPANESE_EUC_JP:
                fromEncoding = "EUC-JP";
                break;
            case Encoding::JAPANESE_SHIFT_JIS:
            case Encoding::KDDI_SHIFT_JIS:
            case Encoding::DOCOMO_SHIFT_JIS:
            case Encoding::SOFTBANK_SHIFT_JIS:
                fromEncoding = "SHIFT_JIS";
                break;
            case Encoding::KDDI_ISO_2022_JP:
            case Encoding::SOFTBANK_ISO_2022_JP:
                fromEncoding = "ISO-2022-JP";
                break;
            case Encoding::CHINESE_BIG5:
                fromEncoding = "BIG5";
                break;
            case Encoding::CHINESE_GB:
                fromEncoding = "GB2312";
                break;
            case Encoding::CHINESE_EUC_DEC:
            case Encoding::CHINESE_EUC_CN:
                fromEncoding = reliable ? "EUC-CN" : "EUC-KR";
                break;
            case Encoding::GBK:
                fromEncoding = "GBK";
                break;
            case Encoding::ISO_2022_CN:
                fromEncoding = "ISO-2022-CN";
                break;
            case Encoding::KOREAN_EUC_KR:
                fromEncoding = "EUC-KR";
                break;
            case Encoding::ISO_2022_KR:
                fromEncoding = "ISO-2022-KR";
                break;
            case Encoding::UTF8:
                fromEncoding = "UTF-8";
                break;
            case Encoding::ASCII_7BIT:
                fromEncoding = "ASCII";
                break;
            case Encoding::RUSSIAN_CP1251:
                fromEncoding = "CP1251";
                break;
            case Encoding::MSFT_CP1252:
                fromEncoding = "CP1252";
                break;
            case Encoding::RUSSIAN_KOI8_RU:
                fromEncoding = "KOI8-U";
                break;
            case Encoding::MSFT_CP1250:
                fromEncoding = "CP1250";
                break;
            case Encoding::ISO_8859_15:
                fromEncoding = "ISO-8859-15";
                break;
            case Encoding::MSFT_CP1254:
                fromEncoding = "CP1254";
                break;
            case Encoding::MSFT_CP1257:
                fromEncoding = "CP1257";
                break;
            case Encoding::ISO_8859_11:
                fromEncoding = "TIS-620";
                break;
            case Encoding::MSFT_CP874:
                fromEncoding = "CP874";
                break;
            case Encoding::MSFT_CP1256:
                fromEncoding = "CP1256";
                break;
            case Encoding::MSFT_CP1255:
                fromEncoding = "CP1255";
                break;
            case Encoding::ISO_8859_8_I:
                fromEncoding = "ISO-8859-8";
                break;
            case Encoding::HEBREW_VISUAL:
                fromEncoding = "ISO-8859-8";
                break;
            case Encoding::MSFT_CP1253:
                fromEncoding = "CP1253";
                break;
            case Encoding::GB18030:
                fromEncoding = "GB18030";
                break;
            case Encoding::BIG5_HKSCS:
                fromEncoding = "BIG5-HKSCS";
                break;
            case Encoding::UTF16BE:
                fromEncoding = "UTF-16BE";
                break;
            case Encoding::UTF16LE:
                fromEncoding = "UTF-16LE";
                break;
            case Encoding::UTF32BE:
                fromEncoding = "UTF-32BE";
                break;
            case Encoding::UTF32LE:
                fromEncoding = "UTF-32LE";
                break;
            case Encoding::BINARYENC:
                fromEncoding = "BINARYENC";
                break;
            case Encoding::HZ_GB_2312:
                fromEncoding = "HZ-GB2312";
                break;
            case Encoding::UTF8UTF8:
                fromEncoding = "UTF-8";
                break;
            case Encoding::UNKNOWN_ENCODING:
            default:
                break;
        }

        // Special cases: upside A
        if (length == 2 && text[0] == static_cast<char>(0xA2) && text[1] == static_cast<char>(0xA3))
            fromEncoding = "EUC-KR";

        iconv_t cd = iconv_open("UTF-8", fromEncoding.c_str());
        if (cd == reinterpret_cast<libiconv_t>(-1))
        {
            std::cerr << "Failed to open iconv for " << fromEncoding << std::endl;
            return sf::String(text);
        }

        std::size_t left = length * sizeof(std::int32_t);
        auto buffer      = std::vector<char>(left);

        auto in  = const_cast<char*>(&text[0]);
        auto out = buffer.data();

        std::size_t result = iconv(cd, &in, &length, &out, &left);

        if (result == -1)
        {
            std::cerr << "Conversion error: " << errno << std::endl;
            iconv_close(cd);

            return sf::String(text);
        }

        iconv_close(cd);
        return sf::String::fromUtf8(buffer.begin(), buffer.end() - left);
#else
        return sf::String(text);
#endif
    }

};