#pragma once

#include <CXO2/Metadata/Graphics/TransformMetadata.hpp>

namespace Cx
{
    struct AvatarMetadata : public TransformMetadata
    {
        Cx::Gender Gender = Gender::Male;
        sf::Vector2f Offset = {};
        std::vector<unsigned int> ItemIDs = {};
    };
}
