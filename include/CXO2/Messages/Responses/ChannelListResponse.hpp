#pragma once

#include <CXO2/Messages/CollectionEnvelope.hpp>
#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct ChannelListResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::GetChannelList;

        struct ChannelState
        {
            std::uint16_t GatewayID  = 0;
            std::uint16_t ID         = 0;
            std::uint32_t Capacity   = 100;
            std::uint32_t UserCount  = 0;
            bool          Active     = false;
        };


        CollectionEnvelope<std::vector<ChannelState>, std::uint32_t> Channels;
    };
}
