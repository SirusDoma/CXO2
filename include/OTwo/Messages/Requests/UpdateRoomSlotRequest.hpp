#pragma once

#include <OTwo/Messages/Commands.hpp>

struct UpdateRoomSlotRequest
{
    static constexpr ::Command Command = RequestCommand::UpdateSlot;

    std::uint8_t ID{};
};
