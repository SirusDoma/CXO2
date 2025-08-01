#pragma once

#include <OTwo/Messages/Commands.hpp>

struct ChannelLoginRequest
{
    static constexpr Command Command = RequestCommand::ChannelLogin;

    std::uint16_t ServerID{};
    std::uint16_t ChannelID{};
};
