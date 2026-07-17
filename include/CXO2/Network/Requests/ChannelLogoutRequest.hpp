#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct ChannelLogoutRequest
    {
        static constexpr Cx::Command Command = RequestCommand::ChannelLogout;
    };
}
