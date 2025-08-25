#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <SFML/System/String.hpp>

struct AnnouncementRequest
{
    static constexpr ::Command Command = RequestCommand::Announce;

    sf::String Content{};
};
