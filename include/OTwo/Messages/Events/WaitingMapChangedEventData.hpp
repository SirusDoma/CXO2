#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <OTwo/Messages/MapInfo.hpp>

struct WaitingMapChangedEventData
{
    static constexpr Command Command = EventCommand::WaitingMapChanged;

    MapInfo Map{};
};
