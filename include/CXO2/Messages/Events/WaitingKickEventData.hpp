#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct WaitingKickEventData
    {
        static constexpr Cx::Command Command = EventCommand::Kick;
    };
}
