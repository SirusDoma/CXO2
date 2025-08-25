#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <SFML/System/String.hpp>

struct WhisperMessageResponse
{
    static constexpr ::Command Command = ResponseCommand::SendWhisper;

    std::uint32_t Invalid{};
    sf::String    Recipient{};
    sf::String    Content{};
};
