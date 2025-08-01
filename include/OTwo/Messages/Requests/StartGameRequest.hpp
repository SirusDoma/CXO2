#pragma once

#include <OTwo/Messages/Commands.hpp>

struct StartGameRequest
{
    static constexpr Command Command = RequestCommand::StartGame;
};
