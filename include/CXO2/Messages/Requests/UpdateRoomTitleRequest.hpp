#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct UpdateRoomTitleRequest
    {
        static constexpr Cx::Command Command = RequestCommand::SetRoomTitle;

        std::string Title{};
    };
}
