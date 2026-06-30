#pragma once

#include <string>
#include <vector>
#include <codecvt>
#include <locale>
#include <stdexcept>

namespace Cx
{
    class AuthTokenDecoder
    {
    public:
        static std::string Decode(const std::string& encodedBase64String)
        {
            return ConvertBigEndianUnicodeToString(DecodeBase64(encodedBase64String));
        }

        static std::vector<unsigned char> DecodeToBytes(const std::string& encodedBase64String)
        {
            return DecodeBase64(encodedBase64String);
        }

    private:
        static bool IsBase64(unsigned char c)
        {
            return std::isalnum(c) || c == '+' || c == '/';
        }

        static std::vector<unsigned char> DecodeBase64(const std::string& encodedString)
        {
            const static std::string base64Chars =
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "0123456789+/";

            int inLen = encodedString.size();
            int i = 0;
            int in = 0;
            unsigned char charArray4[4], charArray3[3];
            std::vector<unsigned char> result;

            while (inLen-- && (encodedString[in] != '=') && IsBase64(encodedString[in]))
            {
                charArray4[i++] = encodedString[in];
                in++;
                if (i == 4)
                {
                    for (i = 0; i < 4; i++)
                    {
                        charArray4[i] = base64Chars.find(charArray4[i]);
                    }

                    charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
                    charArray3[1] = ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2);
                    charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];

                    for (i = 0; (i < 3); i++)
                    {
                        result.push_back(charArray3[i]);
                    }
                    i = 0;
                }
            }

            if (i)
            {
                for (int j = i; j < 4; j++)
                {
                    charArray4[j] = 0;
                }

                for (int j = 0; j < 4; j++)
                {
                    charArray4[j] = base64Chars.find(charArray4[j]);
                }

                charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
                charArray3[1] = ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2);
                charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];

                for (int j = 0; (j < i - 1); j++)
                {
                    result.push_back(charArray3[j]);
                }
            }

            return result;
        }

        static std::string Utf16ToUtf8(const std::vector<std::uint16_t>& utf16)
        {
            std::string utf8;

            for (size_t i = 0; i < utf16.size(); ++i)
            {
                uint32_t codepoint = utf16[i];

                // Handle surrogate pairs
                if (codepoint >= 0xD800 && codepoint <= 0xDBFF)
                {
                    // High surrogate
                    if (i + 1 < utf16.size())
                    {
                        uint16_t low = utf16[i + 1];
                        if (low >= 0xDC00 && low <= 0xDFFF)
                        {
                            // Valid surrogate pair
                            codepoint = 0x10000 + ((codepoint & 0x3FF) << 10) + (low & 0x3FF);
                            ++i; // Skip the low surrogate
                        }
                    }
                }

                // Convert to UTF-8
                if (codepoint <= 0x7F)
                {
                    utf8.push_back(static_cast<char>(codepoint));
                }
                else if (codepoint <= 0x7FF)
                {
                    utf8.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
                    utf8.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
                }
                else if (codepoint <= 0xFFFF)
                {
                    utf8.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
                    utf8.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
                    utf8.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
                }
                else if (codepoint <= 0x10FFFF)
                {
                    utf8.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
                    utf8.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
                    utf8.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
                    utf8.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
                }
            }

            return utf8;
        }

        static std::string ConvertBigEndianUnicodeToString(const std::vector<unsigned char>& data)
        {
            if (data.size() % 2 != 0)
            {
                throw std::invalid_argument("Data length must be even for UTF-16");
            }

            std::vector<std::uint16_t> utf16Chars;
            utf16Chars.reserve(data.size() / 2);

            // Convert big-endian bytes to UTF-16 characters
            for (size_t i = 0; i < data.size(); i += 2)
            {
                std::uint16_t ch = (static_cast<std::uint16_t>(data[i]) << 8) | static_cast<std::uint16_t>(data[i + 1]);
                utf16Chars.push_back(ch);
            }

            // Convert UTF-16 to UTF-8 string
            return Utf16ToUtf8(utf16Chars);
        }
    };
}
