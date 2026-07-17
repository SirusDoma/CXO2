#pragma once

#include <CXO2/Services/Service.hpp>
#include <CXO2/Network/MessageEnvelope.hpp>
#include <CXO2/Network/Responses/AuthResponse.hpp>

#include <Genode/System/Exception.hpp>

#include <cstdint>
#include <string>
#include <functional>

namespace Cx
{
    enum class MusicHall : std::uint8_t;

    struct AuthRequest;

    class SessionContext;
    class CommandLineContext;
    class AuthService : public Service
    {
    public:
        virtual void Authenticate(
            MusicHall server,
            const AuthRequest& request,
            const MessageCallback<AuthResponse>& callback
        ) const = 0;
    };

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
