#include <CXO2/Utilities/StringTranscoder.hpp>

#include <ced/compact_enc_det.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <iconv.h>
#include <stdexcept>
#include <vector>
#include <cstring>
#endif

namespace Cx
{
    std::string StringTranscoder::DetectEncoding(const std::string& text)
    {
        return DetectEncoding(text.c_str(), text.size());
    }

    std::string StringTranscoder::DetectEncoding(const char* text, const std::size_t length)
    {
        bool isReliable = false;
        int bytesConsumed = 0;

        // Compact Encoding Detector result
        Encoding detected = CompactEncDet::DetectEncoding(
            text,
            static_cast<int>(length),
            nullptr,
            nullptr,
            nullptr,
            UNKNOWN_ENCODING,
            UNKNOWN_LANGUAGE,
            CompactEncDet::QUERY_CORPUS,
            true,
            &bytesConsumed,
            &isReliable
        );

        // Custom handling (fallback, reliabilities, etc)
        if (detected == UNKNOWN_ENCODING)
            detected = Encoding::KOREAN_EUC_KR;
        else if (detected == Encoding::ISO_8859_1)
            detected = isReliable ? detected : Encoding::KOREAN_EUC_KR;
        // else if (detected == Encoding::GBK || detected == Encoding::GB18030 || detected == Encoding::HZ_GB_2312 || detected == Encoding::CHINESE_GB)
        //     detected = isReliable ? detected : Encoding::KOREAN_EUC_KR;

        return MimeEncodingName(detected);
    }


    sf::String StringTranscoder::Transcode(const std::string& text)
    {
        return Transcode(text, DetectEncoding(text.data(), text.size()));
    }

    sf::String StringTranscoder::Transcode(const std::string& text, const std::string& encoding)
    {
        return Transcode(text.c_str(), text.size(), encoding);
    }

    sf::String StringTranscoder::Transcode(const char* text, const std::size_t length)
    {
        return Transcode(text, length, DetectEncoding(text, length));
    }

    sf::String StringTranscoder::Transcode(const char* text, std::size_t length, const std::string& encoding)
    {
        for (std::size_t i = 0; i < length; i++)
        {
            if (text[i] == '\0')
            {
                length = i;
                break;
            }
        }

        std::string actualEncoding = encoding;
        if (encoding.empty())
            actualEncoding = DetectEncoding(text, length);

#if defined(_WIN32)
        // Map encoding name to Windows Code Page
        UINT codePage = CP_ACP;
        if (actualEncoding == "EUC-KR") codePage = 949;
        else if (actualEncoding == "SHIFT_JIS") codePage = 932;
        else if (actualEncoding == "GBK" || actualEncoding == "GB2312" || actualEncoding == "GB18030") codePage = 936;
        else if (actualEncoding == "BIG5") codePage = 950;
        else if (actualEncoding == "UTF-8") codePage = CP_UTF8;

        const int wideLen = MultiByteToWideChar(codePage, 0, text, static_cast<int>(length), nullptr, 0);
        if (wideLen <= 0)
            return sf::String();

        std::wstring wideStr(wideLen, L'\0');
        MultiByteToWideChar(codePage, 0, text, static_cast<int>(length), &wideStr[0], wideLen);

        return sf::String(wideStr);

#else
        iconv_t cd = iconv_open("UTF-32LE", actualEncoding.c_str());
        if (cd == (iconv_t)(-1))
            return sf::String();

        std::size_t inBytesLeft = length;
        char* inBuf = const_cast<char*>(text);

        std::size_t outBytesLeft = length * 4 + 4;
        std::vector<char> outBuf(outBytesLeft);
        char* outPtr = outBuf.data();

        std::size_t result = iconv(cd, &inBuf, &inBytesLeft, &outPtr, &outBytesLeft);
        iconv_close(cd);

        if (result == (std::size_t)-1)
            return sf::String();

        std::size_t produced = outBuf.size() - outBytesLeft;
        std::u32string u32Str((char32_t*)outBuf.data(), produced / 4);

        return sf::String(u32Str);
#endif
    }
}
