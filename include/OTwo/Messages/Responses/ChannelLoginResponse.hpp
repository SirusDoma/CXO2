#pragma once

#include <OTwo/Messages/Commands.hpp>

struct ChannelLoginResponse
{
    static constexpr ::Command Command = ResponseCommand::ChannelLogin;

    std::uint32_t Full{};
};
