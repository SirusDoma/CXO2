#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct AuthRequest
    {
        static constexpr Cx::Command Command = RequestCommand::Authorize;

        std::string Token{};
    };
}
