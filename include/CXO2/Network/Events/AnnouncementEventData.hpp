#pragma once

#include <CXO2/Network/Commands.hpp>

#include <string>

namespace Cx
{
    struct AnnouncementEventData
    {
        static constexpr Cx::Command Command = ResponseCommand::Announcement;

        std::string Content{};
    };
}
