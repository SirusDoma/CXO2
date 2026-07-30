#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct SyncMusicDownloadRequest
    {
        static constexpr Cx::Command Command = RequestCommand::SyncMusicDownload;

        std::uint32_t MusicID{};
    };
}
