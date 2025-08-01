#pragma once

#include <OTwo/Messages/Commands.hpp>

struct UpdateMemberReadyStateRequest
{
    static constexpr Command Command = RequestCommand::Ready;
};
