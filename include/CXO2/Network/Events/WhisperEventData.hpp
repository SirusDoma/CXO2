#pragma once

#include <CXO2/Network/Commands.hpp>
#include <SFML/System/String.hpp>

namespace Cx
{
    struct WhisperEventData
    {
        static constexpr Cx::Command Command = EventCommand::ReceiveWhisper;

        sf::String Sender{};
        sf::String Content{};
    };
}
