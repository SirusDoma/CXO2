#ifndef O2JAM_NUMBER_METADATA_HPP
#define O2JAM_NUMBER_METADATA_HPP

#include <SFML/Graphics/Rect.hpp>
#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>

#include <unordered_map>

struct NumberMetadata : public TransformMetadata
{
    sf::Color    Color;
    sf::Vector2u DigitSize;
    float        Kerning;
    unsigned int Value;
    unsigned int DigitCount;
    sf::Time     Duration;
    Gx::Number::Alignment Alignment;
    Gx::BlendMode BlendMode;
    std::unordered_map<unsigned int, std::vector<sf::IntRect>> DigitFrames = {};
};

#endif
