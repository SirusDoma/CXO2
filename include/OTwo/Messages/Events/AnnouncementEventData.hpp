#pragma once

#include <OTwo/Messages/Commands.hpp>

#include <string>

struct AnnouncementEventData
{
    static constexpr Command Command = ResponseCommand::Announcement;

    std::string Content{};
};
