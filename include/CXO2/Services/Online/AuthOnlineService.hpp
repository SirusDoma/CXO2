#pragma once

#include <CXO2/Services/AuthService.hpp>

namespace Cx
{
    class SessionContext;
    class CommandLineContext;

    class NetworkService;
    class AuthOnlineService : public AuthService
    {
    public:
        AuthOnlineService(NetworkService& network, SessionContext& session, CommandLineContext& args);

        void Authenticate(
            MusicHall gateway,
            const AuthRequest& request,
            const MessageCallback<AuthResponse>& callback
        ) const override;

    private:
        NetworkService& m_network;
        SessionContext& m_session;
        CommandLineContext& m_args;
    };
}
