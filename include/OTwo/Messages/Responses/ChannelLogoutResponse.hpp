#pragma once

#include <OTwo/Messages/Commands.hpp>

struct ChannelLogoutResponse
{
    static constexpr Command Command = ResponseCommand::ChannelLogout;

    std::uint32_t Invalid{};
};
