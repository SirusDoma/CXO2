#pragma once

#include <OTwo/Messages/Commands.hpp>

struct ExitPlayingRequest
{
    static constexpr Command Command = RequestCommand::ExitPlaying;
};
