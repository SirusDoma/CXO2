#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <SFML/System/String.hpp>

struct WaitingAdminMessageResponse
{
    static constexpr ::Command Command = ResponseCommand::WaitingAdminMessage;

    sf::String Sender{};
    sf::String Content{};
};
