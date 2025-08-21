#pragma once

#include <OTwo/Messages/Commands.hpp>

struct ExitWaitingRequest
{
    static constexpr ::Command Command = RequestCommand::ExitWaiting;
};
