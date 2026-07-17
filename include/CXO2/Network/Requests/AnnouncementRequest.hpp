#pragma once

#include <CXO2/Network/Commands.hpp>
#include <SFML/System/String.hpp>

namespace Cx
{
    struct AnnouncementRequest
    {
        static constexpr Cx::Command Command = RequestCommand::Announce;

        sf::String Content{};
    };
}
