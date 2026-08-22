#pragma once

#include <CXO2/Metadata/Graphics/SpriteMetadata.hpp>

#include <CXO2/UI/Image.hpp>

namespace Cx
{
    struct ImageMetadata : public SpriteMetadata
    {
        struct FrameMetadata
        {
            std::string Name = std::string();
            Image::Frame Value = {};

            std::optional<std::uint16_t> ID = std::nullopt;
        };

        Image::SizeMode SizeMode = Image::SizeMode::Normal;
        sf::FloatRect Bounds = sf::FloatRect();
        std::vector<FrameMetadata> Frames = {};
    };
}
