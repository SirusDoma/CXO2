#pragma once

#include <SFML/System/String.hpp>
#include <string>
#include <cstddef>

class StringTranscoder
{
public:
    static sf::String Transcode(const std::string& text);
    static sf::String Transcode(const char* text, std::size_t length);

    static sf::String Transcode(const std::string& text, const std::string& encoding);
    static sf::String Transcode(const char* text, std::size_t length, const std::string& encoding);

    static std::string DetectEncoding(const std::string& text);
    static std::string DetectEncoding(const char* text, const std::size_t length);
};