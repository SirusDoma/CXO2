#pragma once

#include <SFML/System/String.hpp>

#include <cstddef>
#include <string>

namespace Cx
{
    class StringTranscoderImpl
    {
    public:
        static sf::String Transcode(const char* text, std::size_t length, const std::string& encoding);
    };
}
