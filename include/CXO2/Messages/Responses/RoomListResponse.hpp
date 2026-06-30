#pragma once

#include <CXO2/Messages/Commands.hpp>
#include <CXO2/Messages/CollectionEnvelope.hpp>
#include <CXO2/Messages/RoomInfo.hpp>

#include <cstdint>

namespace Cx
{
    struct RoomListResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::GetRoomList;

        CollectionEnvelope<std::vector<RoomInfo>, std::uint32_t> Rooms{0, 100};
    };
}
