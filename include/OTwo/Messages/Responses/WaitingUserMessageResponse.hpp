#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <SFML/System/String.hpp>

struct WaitingUserMessageResponse
{
    static constexpr ::Command Command = ResponseCommand::WaitingUserMessage;

    sf::String Sender{};
    sf::String Content{};
};
