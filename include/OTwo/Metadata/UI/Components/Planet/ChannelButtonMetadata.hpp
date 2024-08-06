#ifndef O2JAM_CHANNEL_BUTTON_METADATA_HPP
#define O2JAM_CHANNEL_BUTTON_METADATA_HPP

#include <OTwo/Metadata/UI/RadioButtonMetadata.hpp>
#include <OTwo/UI/Planet/ChannelButton.hpp>
#include <OTwo/Metadata/ResourceMetadata.hpp>

#include <Genode/IO/Json.hpp>

#include <unordered_map>

struct ChannelButtonMetadata : public RadioButtonMetadata
{
    std::unordered_map<ChannelButton::Mode, std::unordered_map<Gx::Control::State, sf::IntRect>> States;
};

#endif