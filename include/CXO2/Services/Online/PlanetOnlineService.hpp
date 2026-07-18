#pragma once

#include <CXO2/Services/PlanetService.hpp>

namespace Cx
{
    class SessionContext;
    class NetworkService;
    class PlanetOnlineService : public PlanetService
    {
    public:
        PlanetOnlineService(NetworkService& network, SessionContext& session);

        void GetChannelList(
            const MessageCallback<ChannelListResponse>& callback
        ) const override;

        void Login(
            const ChannelLoginRequest& request,
            const MessageCallback<ChannelLoginResponse>& callback
        ) const override;

    private:
        NetworkService& m_network;
        SessionContext& m_session;
    };
}
