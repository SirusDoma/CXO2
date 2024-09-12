#ifndef O2JAM_EQUALIZER_METADATA_HPP
#define O2JAM_EQUALIZER_METADATA_HPP

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>

struct EqualizerMetadata : public TransformMetadata
{
    unsigned int Count;
    float Spacing;
    ResourceType ItemType = ResourceType::None;
    std::string ItemName = std::string();
    Gx::Json ItemSource;
};

#endif
