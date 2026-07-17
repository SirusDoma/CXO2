#pragma once

#include <CXO2/Network/Commands.hpp>
#include <SFML/System/String.hpp>

namespace Cx
{
    struct MainRoomAdminMessageResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::MainRoomAdminMessage;

        sf::String Sender{};
        sf::String Content{};
    };
}
