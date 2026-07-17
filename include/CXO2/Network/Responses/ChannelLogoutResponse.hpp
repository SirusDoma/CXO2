#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct ChannelLogoutResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::ChannelLogout;

        std::uint32_t Invalid{};
    };
}
