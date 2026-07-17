#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct MemberMusicLoadedEventData
    {
        static constexpr Cx::Command Command = EventCommand::MusicLoaded;

        std::uint8_t ID{};
    };
}
