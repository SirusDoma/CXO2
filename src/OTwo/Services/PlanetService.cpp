#include <OTwo/Services/PlanetService.hpp>

#include <OTwo/Contexts/SessionContext.hpp>

#include <OTwo/Network/NetworkAdapter.hpp>
#include <OTwo/Messages/Requests/ChannelListRequest.hpp>
#include <OTwo/Messages/Requests/ChannelLoginRequest.hpp>
#include <OTwo/Messages/Requests/SendMusicListRequest.hpp>

#include <OTwo/Messages/Responses/ChannelListResponse.hpp>
#include <OTwo/Messages/Responses/ChannelLoginResponse.hpp>

PlanetOnlineService::PlanetOnlineService(NetworkAdapter& adapter, SessionContext& session) :
    m_adapter(adapter),
    m_session(session)
{
}

void PlanetOnlineService::GetChannelList(
    const std::function<void(const ChannelListResponse&)> callback,
    const std::function<void(const NetworkException&)> errorCallback
) const
{
    m_adapter.Exchange<ChannelListRequest, ChannelListResponse>(
        ChannelListRequest{},
        callback,
        errorCallback
    );
}

void PlanetOnlineService::Login(
    const ChannelLoginRequest& request,
    const std::function<void(const ChannelLoginResponse&)> callback,
    const std::function<void(const NetworkException&)> errorCallback
) const
{
    m_adapter.Exchange<ChannelLoginRequest, ChannelLoginResponse>(request, [=] (const ChannelLoginResponse& response)
    {
        if (!response.Full)
        {
            auto list = std::vector<std::uint32_t>();
            for (auto& header : m_session.GetInstalledMusic())
                list.push_back(header.ID);

            const auto status = m_adapter.Send(SendMusicListRequest{ list });
            if (status != sf::Socket::Status::Done)
            {
                if (errorCallback)
                    errorCallback(ConnectionException(status));

                return;
            }
        }

        callback(response);
    }, errorCallback);
}

void PlanetOfflineService::GetChannelList(
    const std::function<void(const ChannelListResponse&)> callback,
    std::function<void(const NetworkException&)> /*onError*/
) const
{
    ChannelListResponse response;
    std::vector<ChannelListResponse::ChannelState> list;
    for (unsigned int x = 0; x < 2; x++)
    {
        for (unsigned int i = 1; i <= 20; i++)
        {
            ChannelListResponse::ChannelState state;
            state.ServerID  = 0;
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
    const std::function<void(const ChannelLoginResponse&)> callback,
    std::function<void(const NetworkException&)> /*onError*/
) const
{
    ChannelLoginResponse response;
    response.Full = 0;
    if (callback)
        callback(response);
}
