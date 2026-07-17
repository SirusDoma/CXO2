#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct RoomRemovedEventData
    {
        static constexpr Cx::Command Command = EventCommand::RoomRemoved;

        std::uint32_t ID{};
    };
}
