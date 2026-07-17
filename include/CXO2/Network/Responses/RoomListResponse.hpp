#pragma once

#include <CXO2/Network/Commands.hpp>
#include <CXO2/Network/CollectionEnvelope.hpp>
#include <CXO2/Network/RoomInfo.hpp>

#include <cstdint>

namespace Cx
{
    struct RoomListResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::GetRoomList;

        CollectionEnvelope<std::vector<RoomInfo>, std::uint32_t> Rooms{0, 100};
    };
}
