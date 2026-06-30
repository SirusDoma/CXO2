#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct PingRequest
    {
        static constexpr Cx::Command Command = GenericCommand::Ping;
    };
}
