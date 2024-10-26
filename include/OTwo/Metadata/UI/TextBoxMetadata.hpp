#pragma once

#include <OTwo/Metadata/UI/LabelMetadata.hpp>
#include <SFML/Graphics/Rect.hpp>

struct TextBoxMetadata : public LabelMetadata
{
    sf::FloatRect Bounds;
    unsigned int  MaximumLength;
    sf::Color     HighlightTextColor, HighlightBackColor;
};
