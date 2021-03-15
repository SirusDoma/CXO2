#ifndef BUTTON_METADATA_HPP
#define BUTTON_METADATA_HPP

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/Json.hpp>

#include <O2/IO/Metadata/SpriteMetadata.hpp>

#include <unordered_map>

struct ButtonMetadata : public SpriteMetadata
{
public:
    ButtonMetadata() {}
    virtual ~ButtonMetadata() {}

    std::unordered_map<Gx::Control::State, SpriteMetadata> States;
};

#endif