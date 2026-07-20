#pragma once

#include <CXO2/Metadata/UI/UiContainerMetadata.hpp>

#include <SFML/Graphics/Color.hpp>

namespace Cx
{
    struct AvatarInfoMetadata : public UiContainerMetadata
    {
        using ColorMap = std::unordered_map<Room::Team, sf::Color>;

        ColorMap TeamColors;
    };
}
