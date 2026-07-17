#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct ExitWaitingRequest
    {
        static constexpr Cx::Command Command = RequestCommand::ExitWaiting;
    };
}
