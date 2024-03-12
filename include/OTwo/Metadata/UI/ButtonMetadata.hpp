#ifndef O2JAM_BUTTON_METADATA_HPP
#define O2JAM_BUTTON_METADATA_HPP

#include <OTwo/Metadata/Graphics/SpriteMetadata.hpp>
#include <Genode/UI/Control.hpp>

#include <unordered_map>

struct ButtonMetadata : public SpriteMetadata
{
    ButtonMetadata() : States() {}
    virtual ~ButtonMetadata() {}

    bool Enabled = true;
    bool Visible = true;
    std::unordered_map<Gx::Control::State, SpriteMetadata> States;
};

#endif