#pragma once

#include <CXO2/Messages/Commands.hpp>
#include <string>

namespace Cx
{
    struct JoinRoomRequest
    {
        static constexpr Cx::Command Command = RequestCommand::JoinWaiting;

        std::uint32_t ID{};
        std::string   Password{};
    };
}
