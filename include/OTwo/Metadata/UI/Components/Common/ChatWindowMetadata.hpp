#pragma once

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>

#include <SFML/Graphics/Rect.hpp>

struct ChatWindowMetadata : public TransformMetadata
{
    sf::FloatRect Bounds;
    unsigned int  FontSize;
    sf::Color     Color;
    unsigned int  MaximumChatsLength;
    float         LineSpacing;
};
