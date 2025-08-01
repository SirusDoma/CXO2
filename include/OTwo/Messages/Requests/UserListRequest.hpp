#pragma once

#include <OTwo/Messages/Commands.hpp>

struct UserListRequest
{
    static constexpr Command Command = RequestCommand::GetUserList;
};
