#pragma once

#include <OTwo/Messages/Commands.hpp>

struct WaitingMemberReadyStateChangedEventData
{
    static constexpr Command Command = EventCommand::UserReadyStateChanged;

    std::uint8_t ID{};
    bool         Ready{};
};
