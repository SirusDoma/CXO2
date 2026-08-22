#pragma once

#include <CXO2/Metadata/UI/RadioButtonMetadata.hpp>
#include <CXO2/UI/Components/Planet/ChannelButton.hpp>
#include <CXO2/Metadata/ResourceMetadata.hpp>

#include <Genode/IO/Json.hpp>

#include <unordered_map>

namespace Cx
{
    struct ChannelButtonMetadata : public RadioButtonMetadata
    {
        std::unordered_map<ChannelButton::Mode, std::unordered_map<Control::State, sf::IntRect>> States;
    };
}
