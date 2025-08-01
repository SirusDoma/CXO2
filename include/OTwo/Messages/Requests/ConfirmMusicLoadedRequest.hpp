#pragma once

#include <OTwo/Messages/Commands.hpp>

struct ConfirmMusicLoadedRequest
{
    static constexpr Command Command = RequestCommand::ConfirmMusicLoaded;
};
