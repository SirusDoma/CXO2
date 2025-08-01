#pragma once

#include <OTwo/Messages/Commands.hpp>

struct WaitingUserMessageResponse
{
    static constexpr Command Command = ResponseCommand::WaitingUserMessage;

    std::string Sender{};
    std::string Content{};
};
