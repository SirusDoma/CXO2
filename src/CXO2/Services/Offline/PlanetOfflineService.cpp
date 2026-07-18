#include <CXO2/Services/Offline/PlanetOfflineService.hpp>

#include <CXO2/Network/Requests/ChannelLoginRequest.hpp>
#include <CXO2/Network/Responses/ChannelListResponse.hpp>
#include <CXO2/Network/Responses/ChannelLoginResponse.hpp>

namespace Cx
{
    void PlanetOfflineService::GetChannelList(
        const MessageCallback<ChannelListResponse>& callback
    ) const
    {
        ChannelListResponse response;
        std::vector<ChannelListResponse::ChannelState> list;
        for (unsigned int x = 0; x < 2; x++)
        {
            for (unsigned int i = 1; i <= 20; i++)
            {
                ChannelListResponse::ChannelState state;
                state.GatewayID = 0;
                state.ID        = static_cast<std::uint16_t>((x * 20) + i);
                state.Capacity  = 100;
                state.UserCount = static_cast<std::uint32_t>((i / 20.f) * 100.f);
                state.Active    = true;
                list.push_back(state);
            }
        }
        response.Channels = list;

        if (callback)
            callback(MessageEnvelope<ChannelListResponse>(response));
    }

    void PlanetOfflineService::Login(
        const ChannelLoginRequest& request,
        const MessageCallback<ChannelLoginResponse>& callback
    ) const
    {
        ChannelLoginResponse response;
        response.Full = 0;

        if (callback)
            callback(MessageEnvelope<ChannelLoginResponse>(response));
    }
}
