#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct WaitingKickEventData
    {
        static constexpr Cx::Command Command = EventCommand::Kick;
    };
}
