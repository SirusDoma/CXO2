#pragma once

#include <OTwo/Messages/Commands.hpp>

struct PingRequest
{
    static constexpr ::Command Command = GenericCommand::Ping;
};
