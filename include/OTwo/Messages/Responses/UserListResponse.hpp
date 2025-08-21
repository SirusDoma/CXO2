#pragma once

#include <OTwo/Messages/Commands.hpp>
#include <OTwo/Messages/CollectionEnvelope.hpp>

#include <cstdint>

struct UserListResponse
{
    static constexpr ::Command Command = ResponseCommand::GetUserList;

    struct UserInfo
    {
        std::string  Name{};
        std::int32_t Level{};
    };

    CollectionEnvelope<std::vector<UserInfo>, std::uint32_t> Users{};
};
