#ifndef O2JAM_TOOLTIP_METADATA_HPP
#define O2JAM_TOOLTIP_METADATA_HPP

#include <OTwo/Metadata/UI/LabelMetadata.hpp>
#include <SFML/System/Time.hpp>

struct ToolTipMetadata : public LabelMetadata
{
    sf::Time     Duration;
    sf::Vector2f Padding;
    float        ContainerOutlineThickness;
    sf::Color    ContainerFillColor;
    sf::Color    ContainerOutlineColor;
};

#endif
