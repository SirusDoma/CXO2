#pragma once

#include <OTwo/Messages/Commands.hpp>

struct MemberMusicLoadedEventData
{
    static constexpr ::Command Command = EventCommand::MusicLoaded;

    std::uint8_t ID{};
};
