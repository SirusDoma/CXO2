#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <SFML/System/String.hpp>

struct MainRoomMessageRequest
{
    static constexpr ::Command Command = RequestCommand::SendMainRoomMessage;

    sf::String Content{};
};
