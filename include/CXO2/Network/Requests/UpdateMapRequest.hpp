#pragma once

#include <CXO2/Network/Commands.hpp>
#include <CXO2/Network/MapInfo.hpp>

namespace Cx
{
    struct UpdateMapRequest
    {
        static constexpr Cx::Command Command = RequestCommand::SetWaitingMap;

        MapInfo Map{};
    };
}
