#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <OTwo/Messages/CollectionEnvelope.hpp>
#include <OTwo/Messages/RoomInfo.hpp>

#include <cstdint>

struct RoomListResponse
{
    static constexpr ::Command Command = ResponseCommand::GetRoomList;

    CollectionEnvelope<std::vector<RoomInfo>, std::uint32_t> Rooms{0, 100};
};
