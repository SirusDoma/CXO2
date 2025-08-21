#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <string>

struct JoinRoomRequest
{
    static constexpr ::Command Command = RequestCommand::JoinWaiting;

    std::uint32_t ID{};
    std::string   Password{};
};
