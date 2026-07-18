#pragma once

#include <CXO2/Services/AuthService.hpp>

namespace Cx
{
    class AuthOfflineService : public AuthService
    {
    public:
        void Authenticate(
            MusicHall server,
            const AuthRequest& request,
            const MessageCallback<AuthResponse>& callback
        ) const override;
    };
}
