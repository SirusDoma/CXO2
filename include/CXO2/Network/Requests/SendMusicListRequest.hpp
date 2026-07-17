#pragma once

#include <CXO2/Network/CollectionEnvelope.hpp>
#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct SendMusicListRequest
    {
        static constexpr Cx::Command Command = RequestCommand::SendMusicList;

        CollectionEnvelope<std::vector<std::uint32_t>, std::uint32_t> MusicIDs{};
    };
}
