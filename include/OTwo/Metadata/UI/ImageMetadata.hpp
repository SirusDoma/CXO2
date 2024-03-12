#ifndef O2JAM_IMAGE_METADATA_HPP
#define O2JAM_IMAGE_METADATA_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Genode/UI/Image.hpp>
#include <OTwo/Metadata/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>

#include <OTwo/Metadata/Graphics/SpriteMetadata.hpp>

#include <map>

struct ImageMetadata : public SpriteMetadata
{
    ImageMetadata() : Frames() {}
    virtual ~ImageMetadata() {}

    std::map<std::string, Gx::Image::Frame> Frames;
};

#endif