#ifndef O2JAM_IMAGE_METADATA_HPP
#define O2JAM_IMAGE_METADATA_HPP

#include <OTwo/Metadata/Graphics/SpriteMetadata.hpp>

#include <Genode/UI/Image.hpp>

struct ImageMetadata : public SpriteMetadata
{
    struct FrameMetadata
    {
        std::string first;
        Gx::Image::Frame second;
    };

    ImageMetadata() : Frames() {}

    std::vector<FrameMetadata> Frames;
};

#endif