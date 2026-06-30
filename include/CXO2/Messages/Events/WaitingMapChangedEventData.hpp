#pragma once

#include <CXO2/Messages/Commands.hpp>
#include <CXO2/Messages/MapInfo.hpp>

namespace Cx
{
    struct WaitingMapChangedEventData
    {
        static constexpr Cx::Command Command = EventCommand::WaitingMapChanged;

        MapInfo Map{};
    };
}
