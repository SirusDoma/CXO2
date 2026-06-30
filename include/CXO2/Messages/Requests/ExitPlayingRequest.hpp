#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct ExitPlayingRequest
    {
        static constexpr Cx::Command Command = RequestCommand::ExitPlaying;
    };
}
