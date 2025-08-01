#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <string>

struct RoomTitleChangedEventData
{
    static constexpr Command Command = EventCommand::RoomTitleChanged;

    std::uint32_t ID{};
    std::string   Title{};
};
