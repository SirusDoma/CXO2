#include <CXO2/Services/AuthService.hpp>

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Contexts/CommandLineContext.hpp>

#include <CXO2/Messages/Auth.hpp>
#include <CXO2/Messages/Requests/AuthRequest.hpp>
#include <CXO2/Messages/Responses/AuthResponse.hpp>
#include <CXO2/Services/MessageService.hpp>

namespace Cx
{
    AuthOnlineService::AuthOnlineService(MessageService& messages, SessionContext& session, CommandLineContext& args) :
        m_messages(messages),
        m_session(session),
        m_args(args)
    {
    }

    void AuthOnlineService::Authenticate(
        MusicHall server,
        const AuthRequest& request,
        const MessageCallback<AuthResponse>& callback
    ) const
    {
        m_messages.Disconnect();
        auto gateways = m_args.GetGatewayInfo();
        const auto it = std::find_if(gateways.begin(), gateways.end(), [server] (const GatewayInfo& info)
        {
            return info.Hall == server;
        });

        if (it == gateways.end())
        {
            callback(std::make_exception_ptr(Gx::ConnectionException(sf::Socket::Status::Disconnected)));
            return;
        }

        m_messages.Connect(it->Address, it->Port, [this, request, callback]
        {
            m_messages.Dispatch(request, callback);
        },
        [callback] (const auto&)
        {
            callback(std::current_exception());
        }, sf::seconds(3));
    }

    void AuthOfflineService::Authenticate(
        MusicHall server,
        const AuthRequest& request,
        const MessageCallback<AuthResponse>& callback
    ) const
    {
        callback(AuthResponse
        {
            AuthResult::Success,
            BillingCode::TB,
        });
    }
}
