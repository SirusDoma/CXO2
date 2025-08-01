#pragma once

#include <OTwo/Messages/Commands.hpp>

struct AuthRequest
{
    static constexpr Command Command = RequestCommand::Authorize;

    std::string Token{};
};
