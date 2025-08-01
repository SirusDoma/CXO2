#pragma once

#include <OTwo/Messages/Commands.hpp>

enum class StartGameResult : std::uint32_t
{
    Success             = 0x00000000, // 0
    NotReady            = 0x00000001, // 1
    TeamUnbalanced      = 0x00000002, // 2
    InsufficientPlayers = 0x00000003, // 3
};

struct StartGameEventData
{
    static constexpr Command Command = EventCommand::StartGame;

    StartGameResult ResultCode{};
};
