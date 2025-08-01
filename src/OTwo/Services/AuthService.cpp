#include <OTwo/Services/AuthService.hpp>
#include <OTwo/Network/NetworkAdapter.hpp>

#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/Contexts/CommandLineContext.hpp>

#include <OTwo/Messages/Auth.hpp>
#include <OTwo/Messages/Requests/AuthRequest.hpp>
#include <OTwo/Messages/Responses/AuthResponse.hpp>

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
