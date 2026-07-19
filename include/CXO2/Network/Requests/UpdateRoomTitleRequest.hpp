#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct UpdateRoomTitleRequest
    {
        static constexpr Cx::Command Command = RequestCommand::SetRoomTitle;

        sf::String Title{};
    };
}
