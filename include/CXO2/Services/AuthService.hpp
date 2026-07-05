#pragma once

#include <CXO2/Services/Service.hpp>
#include <CXO2/Messages/MessageEnvelope.hpp>
#include <CXO2/Messages/Responses/AuthResponse.hpp>

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
    class NetworkException;
    class AuthService : public Service
    {
    public:
        virtual void Authenticate(
            MusicHall server,
            const AuthRequest& request,
            const MessageCallback<AuthResponse>& callback
        ) const = 0;
    };

    class MessageService;
    class AuthOnlineService : public AuthService
    {
    public:
        AuthOnlineService(MessageService& messages, SessionContext& session, CommandLineContext& args);

        void Authenticate(
            MusicHall server,
            const AuthRequest& request,
            const MessageCallback<AuthResponse>& callback
        ) const override;

    private:
        MessageService& m_messages;
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
