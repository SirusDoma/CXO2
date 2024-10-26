#pragma once

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>

struct AvatarMetadata : public TransformMetadata
{
    ::Gender Gender = Gender::Male;
    std::vector<unsigned int> ItemIDs = {};
};
