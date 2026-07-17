#include <CXO2/Services/AuthService.hpp>

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Contexts/CommandLineContext.hpp>

#include <CXO2/Network/Auth.hpp>
#include <CXO2/Network/Requests/AuthRequest.hpp>
#include <CXO2/Network/Responses/AuthResponse.hpp>
#include <CXO2/Services/NetworkService.hpp>

namespace Cx
{
    AuthOnlineService::AuthOnlineService(NetworkService& network, SessionContext& session, CommandLineContext& args) :
        m_network(network),
        m_session(session),
        m_args(args)
    {
    }

    void AuthOnlineService::Authenticate(
        MusicHall gateway,
        const AuthRequest& request,
        const MessageCallback<AuthResponse>& callback
    ) const
    {
        m_network.Disconnect();
        auto gateways = m_args.GetGatewayInfo();
        const auto it = std::find_if(gateways.begin(), gateways.end(), [gateway] (const GatewayInfo& info)
        {
            return info.Hall == gateway;
        });

        if (it == gateways.end())
        {
            callback(std::make_exception_ptr(Gx::ConnectionException(sf::Socket::Status::Disconnected)));
            return;
        }

        m_network.Connect(it->Address, it->Port, [this, request, callback]
        {
            m_network.Dispatch(request, callback);
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
