#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct ChannelInfoRequest
    {
        static constexpr Cx::Command Command = RequestCommand::GetChannelInfo;
    };
}
