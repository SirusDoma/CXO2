#ifndef O2JAM_NUMBER_METADATA_HPP
#define O2JAM_NUMBER_METADATA_HPP

#include <SFML/Graphics/Rect.hpp>
#include <OTwo/Metadata/TransformMetadata.hpp>

#include <unordered_map>

struct NumberMetadata : public TransformMetadata
{
    NumberMetadata() : DigitFrames() {}

    sf::Color    Color;
    sf::Vector2u DigitSize;
    float        LetterSpacing;
    unsigned int Value;
    unsigned int DigitCount;
    std::unordered_map<unsigned int, sf::IntRect>  DigitFrames;
};

#endif
