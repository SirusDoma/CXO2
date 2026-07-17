#include <CXO2/Utilities/Impl/Unix/StringTranscoderImpl.hpp>

#include <iconv.h>

#include <vector>

namespace Cx
{
    sf::String StringTranscoderImpl::Transcode(const char* text, const std::size_t length, const std::string& encoding)
    {
        iconv_t cd = iconv_open("UTF-32LE", encoding.c_str());
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
    }
}
