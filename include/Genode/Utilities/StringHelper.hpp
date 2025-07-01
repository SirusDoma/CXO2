#pragma once

#include <SFML/System/String.hpp>

#include <sstream>
#include <iomanip>
#include <cmath>
#include <regex>

namespace Gx
{
    class StringHelper
    {
    public:
        template<typename T>
        static std::string ToString(const T& value)
        {
            if constexpr (std::is_same_v<T, std::string>)
            {
                return value;
            }
            else if constexpr (std::is_same_v<T, sf::String>)
            {
                return value.toAnsiString();
            }
            else
            {
                std::stringstream ss;
                ss << value;

                return ss.str();
            }
        }

        static bool EqualsCaseInsensitive(const std::string& a, const std::string& b)
        {
            if (a.length() != b.length())
                return false;

            for (size_t i = 0; i < a.length(); ++i)
            {
                if (std::tolower(a[i]) != std::tolower(b[i]))
                {
                    return false;
                }
            }

            return true;
        }

        static sf::String ToPascalCase(const sf::String& input)
        {
            sf::String result = "";
            bool transform = true;

            for (size_t i = 0; i < input.getSize(); i++)
            {
                char curChar = input[i];
                if (transform)
                {
                    result += sf::String(static_cast<char>(std::toupper(curChar)));
                    transform = false;
                }
                else if (curChar == L' ' || curChar == L'_' || curChar == L'-')
                    transform = true;
                else
                    result += curChar;
            }
            return result;
        }

        static sf::String TrimStart(const sf::String& input)
        {
            sf::String result = input;
            for (size_t i = 0; i < result.getSize(); i++)
            {
                if (result[i] != L'\0' && result[i] != L' ' && result[i] != L'\t' && result[i] != L'\n')
                {
                    result = input.substring(i);
                    break;
                }
            }

            return result;
        }

        static sf::String TrimEnd(const sf::String& input)
        {
            sf::String result = input;
            for (size_t i = result.getSize() - 1; i > 0 && i < result.getSize(); i--)
            {
                if (result[i] != L'\0' && result[i] != L' ' && result[i] != L'\t' && result[i] != L'\n')
                {
                    result = input.substring(0, i + 1);
                    break;
                }
            }

            return result;
        }

        static sf::String Trim(const sf::String& input)
        {
            return TrimEnd(TrimStart(input));
        }

        static bool IsGlobMatch(const std::string& input, const sf::String& pattern)
        {
            std::string regexPattern;
            for (char c : pattern)
            {
                if (c == '*')
                    regexPattern += ".*";
                else if (c == '?')
                    regexPattern += ".";
                else if (std::ispunct(c))
                {
                    regexPattern += "\\";
                    regexPattern += c;
                }
                else
                    regexPattern += c;
            }

            auto regex = std::regex(regexPattern);
            return std::regex_match(input, regex);
        }

        static sf::String RemoveExtension(const std::string& fileName)
        {
            if (fileName == "." || fileName == "..")
                return fileName;

            auto pos = fileName.find_last_of("\\/.");
            if (pos != std::string::npos && fileName[pos] == '.')
                return fileName.substr(0, pos);

            return fileName;
        }

        static sf::String ToString(const int value, const int totalLength = 0)
        {
            int threshold = static_cast<int>(pow(10, totalLength));
            if (value < threshold)
            {
                sf::String result = std::to_string(value);
                while (result.getSize() < totalLength)
                    result = "0" + result;

                return result;
            }

            return std::to_string(value);
        }

        static sf::String ToString(const float value, const int precision = 0)
        {
            if (precision == 0)
                return std::to_string(value);

            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << value;

            return oss.str();
        }

        template<typename T>
        static std::string GetTypeName(T& obj, const bool withNamespace = true)
        {
            auto name = std::string(typeid(obj).name());
            if (!withNamespace)
            {
                if (const auto pos = name.find_last_of(':'); pos != std::string::npos)
                    name = name.substr(pos + 1);
            }

            if (name.find(' ') != std::string::npos)
            {
                std::istringstream iss(name);
                std::vector<std::string> parts;
                std::string word;

                while (iss >> word) {
                    parts.push_back(word);
                }

                if (StartsWith(parts.back(), "*") || StartsWith(parts.back(), "&"))
                    parts.pop_back();

                return parts.back();
            }

            if (!name.empty() && std::isdigit(name[0]))
            {
                size_t i = 0;
                while (i < name.length() && std::isdigit(name[i])) {
                    ++i;
                }
                return name.substr(i);
            }

            return name;
        }

        template<typename T>
        static std::string GetTypeName(const bool withNamespace = true)
        {
            auto name = std::string(typeid(T).name());
            if (!withNamespace)
            {
                if (const auto pos = name.find_last_of(':'); pos != std::string::npos)
                    name = name.substr(pos + 1);
            }

            if (name.find(' ') != std::string::npos)
            {
                std::istringstream iss(name);
                std::vector<std::string> parts;
                std::string word;

                while (iss >> word) {
                    parts.push_back(word);
                }

                if (StartsWith(parts.back(), "*") || StartsWith(parts.back(), "&"))
                    parts.pop_back();

                return parts.back();
            }

            if (!name.empty() && std::isdigit(name[0]))
            {
                size_t i = 0;
                while (i < name.length() && std::isdigit(name[i])) {
                    ++i;
                }
                return name.substr(i);
            }

            return name;
        }

        static std::string GetTypeName(const std::type_info& type, const bool withNamespace = true)
        {
            auto name = std::string(type.name());
            if (!withNamespace)
            {
                if (const auto pos = name.find_last_of(':'); pos != std::string::npos)
                    name = name.substr(pos + 1);
            }

            if (name.find(' ') != std::string::npos)
            {
                std::istringstream iss(name);
                std::vector<std::string> parts;
                std::string word;

                while (iss >> word) {
                    parts.push_back(word);
                }

                if (StartsWith(parts.back(), "*") || StartsWith(parts.back(), "&"))
                    parts.pop_back();

                return parts.back();
            }

            if (!name.empty() && std::isdigit(name[0]))
            {
                size_t i = 0;
                while (i < name.length() && std::isdigit(name[i])) {
                    ++i;
                }
                return name.substr(i);
            }

            return name;
        }

        static bool StartsWith(const std::string& string, const std::string& prefix)
        {
            return string.rfind(prefix, 0) != std::string::npos;
        }

        static bool EndsWith(const std::string& string, const std::string& suffix)
        {
            return suffix.size() <= string.size() && string.rfind(suffix, string.size() - suffix.size()) == string.size() - suffix.size();
        }
    };
}
