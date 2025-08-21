#pragma once

#include <OTwo/Messages/Commands.hpp>

struct MainRoomUserMessageResponse
{
    static constexpr ::Command Command = ResponseCommand::MainRoomUserMessage;

    std::string Sender{};
    std::string Content{};
};
