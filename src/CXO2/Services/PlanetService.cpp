#include <CXO2/Services/PlanetService.hpp>

#include <CXO2/Contexts/SessionContext.hpp>

#include <CXO2/Services/NetworkService.hpp>
#include <CXO2/Network/Requests/ChannelListRequest.hpp>
#include <CXO2/Network/Requests/ChannelLoginRequest.hpp>
#include <CXO2/Network/Requests/SendMusicListRequest.hpp>

#include <CXO2/Network/Responses/ChannelListResponse.hpp>
#include <CXO2/Network/Responses/ChannelLoginResponse.hpp>

namespace Cx
{
    PlanetOnlineService::PlanetOnlineService(NetworkService& network, SessionContext& session) :
        m_network(network),
        m_session(session)
    {
    }

    void PlanetOnlineService::GetChannelList(
        const MessageCallback<ChannelListResponse>& callback
    ) const
    {
        m_network.Dispatch<ChannelListRequest, ChannelListResponse>(
            ChannelListRequest{},
            callback
        );
    }

    void PlanetOnlineService::Login(
        const ChannelLoginRequest& request,
        const MessageCallback<ChannelLoginResponse>& callback
    ) const
    {
        m_network.Dispatch<ChannelLoginRequest, ChannelLoginResponse>(request, [this, callback] (const MessageEnvelope<ChannelLoginResponse>& envelope)
        {
            try
            {
                const auto& response = envelope.Open();
                if (!response.Full)
                {
                    auto list = std::vector<std::uint32_t>();
                    for (auto& header : m_session.GetInstalledMusic())
                        list.push_back(header.ID);

                    m_network.Dispatch<SendMusicListRequest>(SendMusicListRequest{ list },
                    [callback, envelope] (const MessageEnvelope<SendMusicListRequest>& result)
                    {
                        try
                        {
                            const auto& _ = result.Open();
                            callback(envelope);
                        }
                        catch (...)
                        {
                            callback(std::current_exception());
                        }
                    });
                }
            }
            catch (...)
            {
                callback(std::current_exception());
            }
        });
    }

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
            callback(response);
    }

    void PlanetOfflineService::Login(
        const ChannelLoginRequest& /*request*/,
        const MessageCallback<ChannelLoginResponse>& callback
    ) const
    {
        ChannelLoginResponse response;
        response.Full = 0;
        if (callback)
            callback(response);
    }
}
