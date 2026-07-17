#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct ConfirmMusicLoadedRequest
    {
        static constexpr Cx::Command Command = RequestCommand::ConfirmMusicLoaded;
    };
}
