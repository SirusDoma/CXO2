#pragma once

#include <OTwo/Messages/Commands.hpp>

struct RoomRemovedEventData
{
    static constexpr Command Command = EventCommand::RoomRemoved;

    std::uint32_t ID{};
};
