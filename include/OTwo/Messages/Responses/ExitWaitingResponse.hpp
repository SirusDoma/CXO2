#pragma once

#include <OTwo/Messages/Commands.hpp>

struct ExitWaitingResponse
{
    static constexpr ::Command Command = ResponseCommand::ExitWaiting;

    std::uint32_t Invalid{};
};
