#pragma once

#include <CXO2/Messages/Commands.hpp>
#include <SFML/System/String.hpp>

namespace Cx
{
    struct WaitingMessageRequest
    {
        static constexpr Cx::Command Command = RequestCommand::SendWaitingMessage;

        sf::String Content{};
    };
}
