#include <CXO2/Services/Online/PlanetOnlineService.hpp>

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
}
