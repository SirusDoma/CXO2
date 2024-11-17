#pragma once

#include <OTwo/Metadata/Graphics/SpriteMetadata.hpp>
#include <Genode/UI/Control.hpp>

#include <unordered_map>

struct ButtonMetadata : public SpriteMetadata
{
    ButtonMetadata() : States() {}

    struct ButtonState : SpriteMetadata
    {
        ButtonState() = default;
        ButtonState(const SpriteMetadata& sprite) : SpriteMetadata(sprite) {}

        sf::Vector2f LocalBounds;
    };

    bool Enabled = true;
    bool Visible = true;
    std::unordered_map<Gx::Control::State, ButtonState> States;
};
