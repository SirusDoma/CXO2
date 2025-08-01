#pragma once

#include <OTwo/Messages/Commands.hpp>

struct MainRoomAdminMessageResponse
{
    static constexpr Command Command = ResponseCommand::MainRoomAdminMessage;

    std::string Sender{};
    std::string Content{};
};
