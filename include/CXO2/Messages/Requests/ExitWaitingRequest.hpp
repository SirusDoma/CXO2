#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct ExitWaitingRequest
    {
        static constexpr Cx::Command Command = RequestCommand::ExitWaiting;
    };
}
