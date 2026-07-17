#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct UpdateMemberReadyStateRequest
    {
        static constexpr Cx::Command Command = RequestCommand::Ready;
    };
}
