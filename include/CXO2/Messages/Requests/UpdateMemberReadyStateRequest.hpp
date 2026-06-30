#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct UpdateMemberReadyStateRequest
    {
        static constexpr Cx::Command Command = RequestCommand::Ready;
    };
}
