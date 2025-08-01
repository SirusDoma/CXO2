#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <OTwo/Models/Room.hpp>

struct WaitingTitleChangedEventData
{
    static constexpr Command Command = EventCommand::WaitingTitleChanged;

    std::string Title{};
};
