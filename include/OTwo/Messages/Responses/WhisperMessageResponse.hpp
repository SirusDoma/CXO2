#pragma once

#include <OTwo/Messages/Commands.hpp>

struct WhisperMessageResponse
{
    static constexpr Command Command = ResponseCommand::SendWhisper;

    std::uint32_t Invalid{};
    std::string   Recipient{};
    std::string   Content{};
};
