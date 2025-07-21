#pragma once

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>

struct AvatarMetadata : public TransformMetadata
{
    ::Gender Gender = Gender::Male;
    sf::Vector2f Offset = {};
    std::vector<unsigned int> ItemIDs = {};
};
