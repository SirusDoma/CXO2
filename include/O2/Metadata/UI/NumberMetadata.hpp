#ifndef O2JAM_NUMBER_METADATA_HPP
#define O2JAM_NUMBER_METADATA_HPP

#include <SFML/Graphics/Rect.hpp>
#include <O2/Metadata/TransformMetadata.hpp>

#include <unordered_map>

struct NumberMetadata : public TransformMetadata
{
    NumberMetadata() : DigitFrames() {}

    sf::Color    Color;
    sf::Vector2u DigitSize;
    float        LetterSpacing;
    std::unordered_map<unsigned int, sf::IntRect>  DigitFrames;
};

#endif
