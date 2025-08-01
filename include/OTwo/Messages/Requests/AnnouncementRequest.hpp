#pragma once

#include <OTwo/Messages/Commands.hpp>

struct AnnouncementRequest
{
    static constexpr Command Command = RequestCommand::Announce;

    std::string Content{};
};
