#pragma once

#include <CXO2/Network/Commands.hpp>
#include <CXO2/Network/MapInfo.hpp>

namespace Cx
{
    struct WaitingMapChangedEventData
    {
        static constexpr Cx::Command Command = EventCommand::WaitingMapChanged;

        MapInfo Map{};
    };
}
