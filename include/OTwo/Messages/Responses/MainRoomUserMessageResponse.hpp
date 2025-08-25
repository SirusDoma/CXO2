#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <SFML/System/String.hpp>

struct MainRoomUserMessageResponse
{
    static constexpr ::Command Command = ResponseCommand::MainRoomUserMessage;

    sf::String Sender{};
    sf::String Content{};
};
