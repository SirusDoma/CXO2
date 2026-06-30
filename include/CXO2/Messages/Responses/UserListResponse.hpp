#pragma once

#include <CXO2/Messages/Commands.hpp>
#include <CXO2/Messages/CollectionEnvelope.hpp>

#include <cstdint>

namespace Cx
{
    struct UserListResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::GetUserList;

        struct UserInfo
        {
            std::string  Name{};
            std::int32_t Level{};
        };

        CollectionEnvelope<std::vector<UserInfo>, std::uint32_t> Users{};
    };
}
