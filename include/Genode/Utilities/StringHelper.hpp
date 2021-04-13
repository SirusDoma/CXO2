#ifndef GENODE_UTILITIES_STRING_HELPER_HPP
#define GENODE_UTILITIES_STRING_HELPER_HPP

#include <SFML/System/String.hpp>

namespace Gx
{
    class StringHelper
    {
    public:
        static sf::String ToPascalCase(const sf::String &input)
        {
            sf::String result = input;
            result[0] = std::toupper(result[0]);

            return result;
        }

        static const sf::String TrimStart(const sf::String &input)
        {
            sf::String result = input;
            for (size_t i = 0; i < result.getSize(); i++)
            {
                if (result[i] == L' ' || result[i] == L'\t' || result[i] == L'\n')
                {
                    result.erase(i, 1);
                    i--;
                }
                else
                    break;
            }

            return result;
        }

        static const sf::String TrimEnd(const sf::String &input)
        {
            sf::String result = input;
            for (size_t i = result.getSize() - 1; i > 0 && !result.isEmpty(); i--)
            {
                if (result[i] == L' ' || result[i] == L'\t' || result[i] == L'\n')
                {
                    result.erase(i, 1);
                    i++;
                }
                else
                    break;
            }

            return result;
        }
        
        static const sf::String Trim(const sf::String &input)
        {
            return TrimEnd(TrimStart(input));
        }
    };
}

#endif
