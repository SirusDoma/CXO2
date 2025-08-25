#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <SFML/System/String.hpp>

struct WhisperMessageRequest
{
    static constexpr ::Command Command = RequestCommand::SendWhisper;

    sf::String Recipient{};
    sf::String Content{};
};
