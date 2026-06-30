#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct ExitWaitingResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::ExitWaiting;

        std::uint32_t Invalid{};
    };
}
