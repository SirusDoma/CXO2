#pragma once

#include <CXO2/Metadata/UI/LabelMetadata.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace Cx
{
    struct InputFieldMetadata : public LabelMetadata
    {
        sf::IntRect   Bounds             = sf::IntRect();
        unsigned int  MaximumLength      = 0;
        sf::Color     HighlightTextColor = sf::Color::Black;
        sf::Color     HighlightBackColor = sf::Color::White;
    };
}
