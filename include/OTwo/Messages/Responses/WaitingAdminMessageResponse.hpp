#pragma once

#include <OTwo/Messages/Commands.hpp>

struct WaitingAdminMessageResponse
{
    static constexpr Command Command = ResponseCommand::WaitingAdminMessage;

    std::string Sender{};
    std::string Content{};
};
