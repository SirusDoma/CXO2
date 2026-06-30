#pragma once

#include <CXO2/Messages/Commands.hpp>
#include <CXO2/Messages/MapInfo.hpp>

namespace Cx
{
    struct UpdateMapRequest
    {
        static constexpr Cx::Command Command = RequestCommand::SetWaitingMap;

        MapInfo Map{};
    };
}
