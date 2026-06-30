#pragma once

#include <CXO2/Messages/Commands.hpp>
#include <CXO2/Models/Room.hpp>

namespace Cx
{
    struct WaitingTitleChangedEventData
    {
        static constexpr Cx::Command Command = EventCommand::WaitingTitleChanged;

        std::string Title{};
    };
}
