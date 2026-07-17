#pragma once

#include <CXO2/Network/Commands.hpp>
#include <SFML/System/String.hpp>

namespace Cx
{
    struct WaitingAdminMessageResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::WaitingAdminMessage;

        sf::String Sender{};
        sf::String Content{};
    };
}
