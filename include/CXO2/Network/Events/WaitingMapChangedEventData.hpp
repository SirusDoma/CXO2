#pragma once

#include <CXO2/Network/Commands.hpp>

#include <cstdint>

namespace Cx
{
    struct WaitingMapChangedEventData
    {
        static constexpr Cx::Command Command = EventCommand::WaitingMapChanged;

        std::uint32_t Map{};
    };
}
