#pragma once

#include <Genode/Graphics/Animation.hpp>
#include <CXO2/Metadata/Graphics/SpriteMetadata.hpp>
#include <SFML/System/Time.hpp>
#include <vector>

namespace Cx
{
    struct AnimationMetadata : SpriteMetadata
    {
        enum class Alignment
        {
            None,
            Center,
            HorizontalCenter,
            VerticalCenter
        };

        struct Frame : Gx::Animation::Frame
        {
            std::optional<std::uint32_t> ID = std::nullopt;
        };

        bool         IsLoop       = false;
        bool         Inherit      = false;
        sf::Time     Duration     = sf::Time::Zero;
        float        Speed        = 1.f;
        unsigned int RepeatCount  = 0;
        Alignment    Align        = Alignment::None;
        std::vector<Frame> Frames = {};
    };
}
