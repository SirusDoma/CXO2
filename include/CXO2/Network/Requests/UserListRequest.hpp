#pragma once

#include <CXO2/Network/Commands.hpp>

namespace Cx
{
    struct UserListRequest
    {
        static constexpr Cx::Command Command = RequestCommand::GetUserList;
    };
}
