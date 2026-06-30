#pragma once

#include <CXO2/Messages/Commands.hpp>

namespace Cx
{
    struct UserListRequest
    {
        static constexpr Cx::Command Command = RequestCommand::GetUserList;
    };
}
