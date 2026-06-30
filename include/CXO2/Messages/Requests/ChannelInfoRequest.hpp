#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct ChannelInfoRequest
    {
        static constexpr Cx::Command Command = RequestCommand::GetChannelInfo;
    };
}
