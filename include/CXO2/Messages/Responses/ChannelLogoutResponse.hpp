#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct ChannelLogoutResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::ChannelLogout;

        std::uint32_t Invalid{};
    };
}
