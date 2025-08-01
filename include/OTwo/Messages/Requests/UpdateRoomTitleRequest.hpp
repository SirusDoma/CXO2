#pragma once

#include <OTwo/Messages/Commands.hpp>

struct UpdateRoomTitleRequest
{
    static constexpr Command Command = RequestCommand::SetRoomTitle;

    std::string Title{};
};
