#pragma once

#include <CXO2/Messages/Commands.hpp>
#include <string>

namespace Cx
{
    struct RoomUserCountChangedEventData
    {
        static constexpr Cx::Command Command = EventCommand::RoomUserCountChanged;

        std::uint32_t ID{};
        std::uint8_t  Capacity{};
        std::uint8_t  UserCount{};
    };
}
