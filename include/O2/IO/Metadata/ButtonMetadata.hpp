#ifndef BUTTON_METADATA_HPP
#define BUTTON_METADATA_HPP

#include <SFML/Graphics.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>
#include <Genode/UI/Button.hpp>

#include <O2/IO/Metadata/SpriteMetadata.hpp>

#include <unordered_map>

class ButtonMetadata : public SpriteMetadata
{
public:
    ButtonMetadata() {}
    virtual ~ButtonMetadata() {}

    std::unordered_map<Gx::ButtonState, SpriteMetadata> States;
};

#endif