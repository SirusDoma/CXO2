#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <SFML/System/String.hpp>

struct MainRoomAdminMessageResponse
{
    static constexpr ::Command Command = ResponseCommand::MainRoomAdminMessage;

    sf::String Sender{};
    sf::String Content{};
};
