#pragma once

#include <CXO2/Services/Service.hpp>

#include <cstdint>
#include <string>
#include <functional>

namespace Cx
{
    enum class MusicHall : std::uint8_t;

    struct AuthResponse;

    class SessionContext;
    class CommandLineContext;
    class NetworkException;
    class AuthService : public Service
    {
    public:
        virtual void Authenticate(
            MusicHall server,
            const std::string& token,
            std::function<void(const AuthResponse& response)> callback,
            std::function<void(const NetworkException&)> errorCallback = nullptr
        ) const = 0;
    };

    class NetworkAdapter;
    class AuthOnlineService : public AuthService
    {
    public:
        explicit AuthOnlineService(NetworkAdapter& adapter, SessionContext& session, CommandLineContext& args);

        void Authenticate(
            MusicHall server,
            const std::string& token,
            std::function<void(const AuthResponse& response)> callback,
            std::function<void(const NetworkException&)> errorCallback = nullptr
        ) const override;

    private:
        NetworkAdapter& m_adapter;
        SessionContext& m_session;
        CommandLineContext& m_args;
    };


    class AuthOfflineService : public AuthService
    {
    public:
        void Authenticate(
            MusicHall server,
            const std::string& token,
            std::function<void(const AuthResponse& response)> callback,
            std::function<void(const NetworkException&)> errorCallback = nullptr
        ) const override;
    };


}
