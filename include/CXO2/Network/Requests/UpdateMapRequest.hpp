#pragma once

#include <CXO2/Network/Commands.hpp>

#include <cstdint>

namespace Cx
{
    struct UpdateMapRequest
    {
        static constexpr Cx::Command Command = RequestCommand::SetWaitingMap;

        std::uint32_t Map{};
    };
}
