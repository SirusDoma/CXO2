#pragma once

#include <OTwo/Messages/Commands.hpp>

struct WaitingKickEventData
{
    static constexpr Command Command = EventCommand::Kick;
};
