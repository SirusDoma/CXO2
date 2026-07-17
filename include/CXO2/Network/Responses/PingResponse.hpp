#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct PingResponse
    {
        static constexpr Cx::Command Command = GenericCommand::Ping;
    };
}
