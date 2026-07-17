#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct ChannelListRequest
    {
        static constexpr Cx::Command Command = RequestCommand::GetChannelList;
    };
}
