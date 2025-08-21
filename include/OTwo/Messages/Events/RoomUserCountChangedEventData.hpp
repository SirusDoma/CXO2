#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <string>

struct RoomUserCountChangedEventData
{
    static constexpr ::Command Command = EventCommand::RoomUserCountChanged;

    std::uint32_t ID{};
    std::uint8_t  Capacity{};
    std::uint8_t  UserCount{};
};
