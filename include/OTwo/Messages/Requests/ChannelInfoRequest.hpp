#pragma once

#include <OTwo/Messages/Commands.hpp>

struct ChannelInfoRequest
{
    static constexpr ::Command Command = RequestCommand::GetChannelInfo;
};
