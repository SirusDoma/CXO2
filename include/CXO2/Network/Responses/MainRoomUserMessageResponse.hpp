#pragma once

#include <CXO2/Network/Commands.hpp>
#include <SFML/System/String.hpp>

namespace Cx
{
    struct MainRoomUserMessageResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::MainRoomUserMessage;

        sf::String Sender{};
        sf::String Content{};
    };
}
