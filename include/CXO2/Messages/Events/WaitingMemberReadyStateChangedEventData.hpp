#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct WaitingMemberReadyStateChangedEventData
    {
        static constexpr Cx::Command Command = EventCommand::UserReadyStateChanged;

        std::uint8_t ID{};
        bool         Ready{};
    };
}
