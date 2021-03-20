#ifndef IMAGE_METADATA_HPP
#define IMAGE_METADATA_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>

#include <O2/IO/Metadata/SpriteMetadata.hpp>

#include <unordered_map>

struct ImageMetadata : public SpriteMetadata
{
public:
    ImageMetadata() {}
    virtual ~ImageMetadata() {}

    std::unordered_map<std::string, sf::IntRect> Frames;
};

#endif