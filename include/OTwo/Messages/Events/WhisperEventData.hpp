#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <SFML/System/String.hpp>

struct WhisperEventData
{
    static constexpr ::Command Command = EventCommand::ReceiveWhisper;

    sf::String Sender{};
    sf::String Content{};
};
