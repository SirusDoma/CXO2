#pragma once

#include <SFML/System/String.hpp>
#include <string>
#include <cstddef>

class StringTranscoder
{
public:
    static sf::String Transcode(const std::string& text);
    static sf::String Transcode(const char* text, std::size_t length);

private:
    static sf::String Transcode(const char* text, std::size_t length, const std::string& encoding);
};