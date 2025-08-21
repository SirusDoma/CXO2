#pragma once

#include <OTwo/Messages/Commands.hpp>

struct WaitingMessageRequest
{
    static constexpr ::Command Command = RequestCommand::SendWaitingMessage;

    std::string Content{};
};
