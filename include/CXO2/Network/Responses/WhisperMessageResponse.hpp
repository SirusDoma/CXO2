#pragma once

#include <CXO2/Network/Commands.hpp>
#include <SFML/System/String.hpp>

namespace Cx
{
    struct WhisperMessageResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::SendWhisper;

        std::uint32_t Invalid{};
        sf::String    Recipient{};
        sf::String    Content{};
    };
}
