#pragma once

#include <CXO2/Messages/Commands.hpp>
#include <SFML/System/String.hpp>

namespace Cx
{
    struct WhisperMessageRequest
    {
        static constexpr Cx::Command Command = RequestCommand::SendWhisper;

        sf::String Recipient{};
        sf::String Content{};
    };
}
