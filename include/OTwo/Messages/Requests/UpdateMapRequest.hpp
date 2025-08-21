#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <OTwo/Messages/MapInfo.hpp>

struct UpdateMapRequest
{
    static constexpr ::Command Command = RequestCommand::SetWaitingMap;

    MapInfo Map{};
};
