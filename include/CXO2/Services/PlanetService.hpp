#pragma once

#include <CXO2/Services/Service.hpp>
#include <CXO2/Network/Responses/ChannelListResponse.hpp>
#include <CXO2/Network/Requests/ChannelLoginRequest.hpp>
#include <CXO2/Network/Responses/ChannelLoginResponse.hpp>

#include <functional>
#include <CXO2/Network/MessageEnvelope.hpp>

namespace Cx
{
    class SessionContext;
    class NetworkService;
    class PlanetService : public Service
    {
    public:
        virtual void GetChannelList(
            const MessageCallback<ChannelListResponse>& callback
        ) const = 0;

        virtual void Login(
            const ChannelLoginRequest& request,
            const MessageCallback<ChannelLoginResponse>& callback
        ) const = 0;
    };

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

    class PlanetOfflineService : public PlanetService
    {
    public:
        void GetChannelList(
            const MessageCallback<ChannelListResponse>& callback
        ) const override;

        void Login(
            const ChannelLoginRequest& request,
            const MessageCallback<ChannelLoginResponse>& callback
        ) const override;
    };
}
