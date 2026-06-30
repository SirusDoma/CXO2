#pragma once

#include <CXO2/Metadata/Graphics/SpriteMetadata.hpp>

#include <Genode/UI/Image.hpp>

namespace Cx
{
    struct ImageMetadata : public SpriteMetadata
    {
        struct FrameMetadata
        {
            std::string Name = std::string();
            Gx::Image::Frame Value = {};

            std::optional<std::uint16_t> ID = std::nullopt;
        };

        Gx::Image::SizeMode SizeMode = Gx::Image::SizeMode::Normal;
        sf::FloatRect Bounds = sf::FloatRect();
        std::vector<FrameMetadata> Frames = {};
    };
}
