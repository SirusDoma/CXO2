#ifndef O2JAM_BUTTON_METADATA_HPP
#define O2JAM_BUTTON_METADATA_HPP

#include <O2/Metadata/SpriteMetadata.hpp>

#include <unordered_map>

struct ButtonMetadata : public SpriteMetadata
{
    ButtonMetadata() : States() {}
    virtual ~ButtonMetadata() {}

    std::unordered_map<Gx::Control::State, SpriteMetadata> States;
};

#endif