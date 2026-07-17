#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct WaitingMemberReadyStateChangedEventData
    {
        static constexpr Cx::Command Command = EventCommand::UserReadyStateChanged;

        std::uint8_t ID{};
        bool         Ready{};
    };
}
