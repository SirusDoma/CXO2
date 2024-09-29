#ifndef O2JAM_AVATAR_METADATA_HPP
#define O2JAM_AVATAR_METADATA_HPP

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>

struct AvatarMetadata : public TransformMetadata
{
    ::Gender Gender = Gender::Male;
    std::vector<unsigned int> ItemIDs = {};
};

#endif
