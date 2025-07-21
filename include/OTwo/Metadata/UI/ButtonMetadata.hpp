#pragma once

#include <OTwo/Metadata/Graphics/SpriteMetadata.hpp>
#include <Genode/UI/Control.hpp>

#include <unordered_map>

struct ButtonMetadata : public SpriteMetadata
{
    struct ButtonState : SpriteMetadata
    {
        ButtonState() = default;
        explicit ButtonState(const SpriteMetadata& sprite)
            : SpriteMetadata(sprite) {}

        sf::IntRect LocalBounds = {};
        std::optional<std::uint16_t> ID = std::nullopt;
    };

    bool Enabled = true;
    bool Visible = true;
    std::unordered_map<Gx::Control::State, ButtonState> States = {};
};
