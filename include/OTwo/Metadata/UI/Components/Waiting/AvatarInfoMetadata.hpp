#pragma once

#include <OTwo/Metadata/UI/UiContainerMetadata.hpp>

#include <SFML/Graphics/Color.hpp>

struct AvatarInfoMetadata : public UiContainerMetadata
{
    using ColorMap = std::unordered_map<RoomTeam, sf::Color>;

    ColorMap TeamColors;
};
