#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct ChannelLoginRequest
    {
        static constexpr Cx::Command Command = RequestCommand::ChannelLogin;

        std::uint16_t ServerID{};
        std::uint16_t ChannelID{};
    };
}
