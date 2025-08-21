#pragma once

#include <OTwo/Messages/Commands.hpp>

struct WhisperEventData
{
    static constexpr ::Command Command = EventCommand::ReceiveWhisper;

    std::string   Sender{};
    std::string   Content{};
};
