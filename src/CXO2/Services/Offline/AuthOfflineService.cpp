#include <CXO2/Services/Offline/AuthOfflineService.hpp>

#include <CXO2/Network/Auth.hpp>
#include <CXO2/Network/Requests/AuthRequest.hpp>
#include <CXO2/Network/Responses/AuthResponse.hpp>

namespace Cx
{
    void AuthOfflineService::Authenticate(
        MusicHall server,
        const AuthRequest& request,
        const MessageCallback<AuthResponse>& callback
    ) const
    {
        const auto response = AuthResponse
        {
            AuthResult::Success,
            BillingCode::TB,
        };

        if (callback)
            callback(MessageEnvelope<AuthResponse>(response));
    }
}
