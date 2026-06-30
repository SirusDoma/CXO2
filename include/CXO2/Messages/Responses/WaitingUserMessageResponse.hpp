#pragma once

#include <CXO2/Messages/Commands.hpp>
#include <SFML/System/String.hpp>

namespace Cx
{
    struct WaitingUserMessageResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::WaitingUserMessage;

        sf::String Sender{};
        sf::String Content{};
    };
}
