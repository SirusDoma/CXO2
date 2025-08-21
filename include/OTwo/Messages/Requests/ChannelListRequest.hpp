#pragma once

#include <OTwo/Messages/Commands.hpp>

struct ChannelListRequest
{
    static constexpr ::Command Command = RequestCommand::GetChannelList;
};
