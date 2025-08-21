#pragma once

#include <OTwo/Messages/Commands.hpp>

struct ChannelLogoutRequest
{
    static constexpr ::Command Command = RequestCommand::ChannelLogout;
};
