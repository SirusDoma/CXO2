#pragma once

#include <OTwo/Metadata/Graphics/TransformMetadata.hpp>

#include <Genode/Graphics/BlendMode.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

struct SpriteMetadata : TransformMetadata
{
    sf::Color     Color     = sf::Color::White;
    sf::IntRect   TexCoords = sf::IntRect();
    Gx::BlendMode BlendMode = Gx::BlendMode::Auto;
    std::optional<std::uint32_t> FrameID = std::nullopt;
};
