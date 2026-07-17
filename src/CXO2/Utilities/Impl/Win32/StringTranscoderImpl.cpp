#include <CXO2/Utilities/Impl/Win32/StringTranscoderImpl.hpp>

#include <windows.h>

namespace Cx
{
    sf::String StringTranscoderImpl::Transcode(const char* text, const std::size_t length, const std::string& encoding)
    {
        // Map encoding name to Windows Code Page
        UINT codePage = CP_ACP;
        if (encoding == "EUC-KR") codePage = 949;
        else if (encoding == "SHIFT_JIS") codePage = 932;
        else if (encoding == "GBK" || encoding == "GB2312" || encoding == "GB18030") codePage = 936;
        else if (encoding == "BIG5") codePage = 950;
        else if (encoding == "UTF-8") codePage = CP_UTF8;

        const int wideLen = MultiByteToWideChar(codePage, 0, text, static_cast<int>(length), nullptr, 0);
        if (wideLen <= 0)
            return sf::String();

        std::wstring wideStr(wideLen, L'\0');
        MultiByteToWideChar(codePage, 0, text, static_cast<int>(length), &wideStr[0], wideLen);

        return sf::String(wideStr);
    }
}
