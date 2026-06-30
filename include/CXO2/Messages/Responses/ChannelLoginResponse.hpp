#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct ChannelLoginResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::ChannelLogin;

        std::uint32_t Full{};
    };
}
