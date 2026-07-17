#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct UpdateRoomSlotRequest
    {
        static constexpr Cx::Command Command = RequestCommand::UpdateSlot;

        std::uint8_t ID{};
    };
}
