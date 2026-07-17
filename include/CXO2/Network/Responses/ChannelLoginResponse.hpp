#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct ChannelLoginResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::ChannelLogin;

        std::uint32_t Full{};
    };
}
