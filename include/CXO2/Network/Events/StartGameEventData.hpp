#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    enum class StartGameResult : std::uint32_t
    {
        Success             = 0x00000000, // 0
        NotReady            = 0x00000001, // 1
        TeamUnbalanced      = 0x00000002, // 2
        InsufficientPlayers = 0x00000003, // 3
    };

    struct StartGameEventData
    {
        static constexpr Cx::Command Command = EventCommand::StartGame;

        StartGameResult ResultCode{};
    };
}
