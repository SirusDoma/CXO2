#ifndef IMAGE_METADATA_HPP
#define IMAGE_METADATA_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>

#include <O2/Metadata/SpriteMetadata.hpp>

#include <unordered_map>

struct ImageMetadata : public SpriteMetadata
{
public:
    ImageMetadata() : m_frames() {}
    virtual ~ImageMetadata() {}

    const std::unordered_map<std::string, sf::IntRect> &GetFrames() const { return m_frames; }
    void AddFrame(const std::string& key, const sf::IntRect& frame) { m_frames[key] = frame; }

private:
    std::unordered_map<std::string, sf::IntRect> m_frames;
};

#endif