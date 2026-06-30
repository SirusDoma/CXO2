#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct ChannelLogoutRequest
    {
        static constexpr Cx::Command Command = RequestCommand::ChannelLogout;
    };
}
