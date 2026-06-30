#include <CXO2/Services/AuthService.hpp>
#include <CXO2/Network/NetworkAdapter.hpp>

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Contexts/CommandLineContext.hpp>

#include <CXO2/Messages/Auth.hpp>
#include <CXO2/Messages/Requests/AuthRequest.hpp>
#include <CXO2/Messages/Responses/AuthResponse.hpp>

namespace Cx
{
    AuthOnlineService::AuthOnlineService(NetworkAdapter& adapter, SessionContext& session, CommandLineContext& args) :
        m_adapter(adapter),
        m_session(session),
        m_args(args)
    {
    }

    void AuthOnlineService::Authenticate(
        MusicHall server,
        const std::string& token,
        const std::function<void(const AuthResponse& response)> callback,
        const std::function<void(const NetworkException&)> errorCallback
    ) const
    {
        return m_adapter.Exchange<AuthRequest, AuthResponse>(AuthRequest{token}, callback, errorCallback);
    }

    void AuthOfflineService::Authenticate(
        MusicHall server,
        const std::string& token,
        const std::function<void(const AuthResponse& response)> callback,
        const std::function<void(const NetworkException&)> errorCallback
    ) const
    {
        callback(AuthResponse
        {
            AuthResult::Success,
            BillingCode::TB,
        });
    }
}
