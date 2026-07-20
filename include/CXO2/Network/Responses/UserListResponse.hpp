#pragma once

#include <CXO2/Network/Commands.hpp>
#include <CXO2/Network/CollectionEnvelope.hpp>

#include <cstdint>

namespace Cx
{
    struct UserListResponse
    {
        static constexpr Cx::Command Command = ResponseCommand::GetUserList;

        struct User
        {
            sf::String   Name{};
            std::int32_t Level{};
        };

        CollectionEnvelope<std::vector<User>, std::uint32_t> Users{};
    };
}
