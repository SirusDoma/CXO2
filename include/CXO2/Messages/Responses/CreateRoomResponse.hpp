#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    enum class CreateRoomResult : std::uint32_t
    {
        Success     = 0x00000000, // 00
        ChannelFull = 0x00000001, // 01
    };

    struct CreateRoomResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::CreateRoom;

        CreateRoomResult ResultCode{};
        std::uint32_t ID{};
    };
}
