#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct ChannelListRequest
    {
        static constexpr Cx::Command Command = RequestCommand::GetChannelList;
    };
}
