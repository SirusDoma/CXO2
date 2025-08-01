#pragma once

#include <OTwo/Messages/Commands.hpp>

struct WhisperMessageRequest
{
    static constexpr Command Command = RequestCommand::SendWhisper;

    std::string Recipient{};
    std::string Content{};
};
