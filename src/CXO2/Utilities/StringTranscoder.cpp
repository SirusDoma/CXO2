#include <CXO2/Utilities/StringTranscoder.hpp>

#include <CXO2/Utilities/StringTranscoderImpl.hpp>

#include <compact_enc_det/compact_enc_det.h>

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

        return StringTranscoderImpl::Transcode(text, length, actualEncoding);
    }
}
