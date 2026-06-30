#pragma once

#include <CXO2/Metadata/Graphics/TransformMetadata.hpp>

#include <SFML/Graphics/Rect.hpp>

namespace Cx
{
    struct ChatWindowMetadata : public TransformMetadata
    {
        sf::IntRect   Bounds = sf::IntRect();
        unsigned int  FontSize = 13;
        sf::Color     Color = sf::Color::White;
        unsigned int  MaximumChatsLength = 5;
        float         LineSpacing = 0;
    };
}
