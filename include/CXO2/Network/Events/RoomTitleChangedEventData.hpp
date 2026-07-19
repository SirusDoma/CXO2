#pragma once

#include <CXO2/Network/Commands.hpp>
#include <string>

namespace Cx
{
    struct RoomTitleChangedEventData
    {
        static constexpr Cx::Command Command = EventCommand::RoomTitleChanged;

        std::uint32_t ID{};
        sf::String    Title{};
    };
}
