#pragma once

#include <CXO2/Messages/Commands.hpp>
#include <SFML/System/String.hpp>

namespace Cx
{
    struct MainRoomMessageRequest
    {
        static constexpr Cx::Command Command = RequestCommand::SendMainRoomMessage;

        sf::String Content{};
    };
}
