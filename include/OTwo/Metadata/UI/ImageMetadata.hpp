#ifndef O2JAM_IMAGE_METADATA_HPP
#define O2JAM_IMAGE_METADATA_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Genode/UI/Image.hpp>
#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>

#include <OTwo/Metadata/SpriteMetadata.hpp>

#include <unordered_map>

struct ImageMetadata : public SpriteMetadata
{
    ImageMetadata() : Frames() {}
    virtual ~ImageMetadata() {}

    std::unordered_map<std::string, Gx::Image::Frame> Frames;
};

#endif