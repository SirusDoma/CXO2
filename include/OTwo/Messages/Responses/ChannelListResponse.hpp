#pragma once

#include <OTwo/Messages/CollectionEnvelope.hpp>
#include <OTwo/Messages/Commands.hpp>

struct ChannelListResponse
{
    static constexpr Command Command = ResponseCommand::GetChannelList;

    struct ChannelState
    {
        std::uint16_t ServerID   = 0;
        std::uint16_t ID         = 0;
        std::uint32_t Capacity   = 100;
        std::uint32_t UserCount  = 0;
        bool          Active     = false;
    };


    CollectionEnvelope<std::vector<ChannelState>, std::uint32_t> Channels;
};
