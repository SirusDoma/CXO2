#pragma once

#include <CXO2/Services/Service.hpp>
#include <CXO2/Network/MessageEnvelope.hpp>
#include <CXO2/Network/Responses/AuthResponse.hpp>

#include <cstdint>
#include <string>
#include <functional>

namespace Cx
{
    enum class MusicHall : std::uint8_t;

    struct AuthRequest;

    class AuthService : public Service
    {
    public:
        virtual void Authenticate(
            MusicHall server,
            const AuthRequest& request,
            const MessageCallback<AuthResponse>& callback
        ) const = 0;
    };
}
