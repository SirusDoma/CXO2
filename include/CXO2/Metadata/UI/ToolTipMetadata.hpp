#pragma once

#include <CXO2/Metadata/UI/LabelMetadata.hpp>
#include <SFML/System/Time.hpp>

namespace Cx
{
    struct ToolTipMetadata : public LabelMetadata
    {
        sf::Time     Duration = sf::seconds(3.0f);
        sf::Vector2f Padding;
        float        ContainerOutlineThickness = 1.f;
        sf::Color    ContainerFillColor = sf::Color::White;
        sf::Color    ContainerOutlineColor = sf::Color::Black;
    };
}
