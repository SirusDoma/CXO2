#pragma once

#include <OTwo/Messages/Commands.hpp>

struct MainRoomMessageRequest
{
    static constexpr Command Command = RequestCommand::SendMainRoomMessage;

    std::string Content{};
};
