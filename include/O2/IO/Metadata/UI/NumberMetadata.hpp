#ifndef NUMBER_METADATA_HPP
#define NUMBER_METADATA_HPP

#include <SFML/Graphics/Rect.hpp>
#include <O2/IO/Metadata/TransformMetadata.hpp>

#include <unordered_map>

class NumberMetadata : public TransformMetadata
{
public:
    sf::Color    Color;
    sf::Vector2u DigitSize;
    float        LetterSpacing;
    std::unordered_map<unsigned int, sf::IntRect> DigitFrames;
};

#endif
