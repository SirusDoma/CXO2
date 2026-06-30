#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct StartGameRequest
    {
        static constexpr Cx::Command Command = RequestCommand::StartGame;
    };
}
