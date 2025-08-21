#pragma once

#include <OTwo/Messages/CollectionEnvelope.hpp>
#include <OTwo/Messages/Commands.hpp>

struct SendMusicListRequest
{
    static constexpr ::Command Command = RequestCommand::SendMusicList;

    CollectionEnvelope<std::vector<std::uint32_t>, std::uint32_t> MusicIDs{};
};
