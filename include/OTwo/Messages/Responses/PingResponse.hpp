#pragma once

#include <OTwo/Messages/Commands.hpp>

struct PingResponse
{
    static constexpr ::Command Command = GenericCommand::Ping;
};
