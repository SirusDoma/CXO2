#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>

#include <unordered_map>

struct BitmapNumberMetadata : public TransformMetadata
{
    sf::Color    Color;
    sf::Vector2u DigitSize;
    float        Kerning;
    unsigned int Value;
    unsigned int DigitCount;
    sf::Time     Duration;
    Gx::BitmapNumber::Alignment Alignment;
    Gx::BlendMode BlendMode;
    std::unordered_map<unsigned int, std::vector<sf::IntRect>> DigitFrames = {};
};
