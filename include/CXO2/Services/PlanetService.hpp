#pragma once

#include <CXO2/Services/Service.hpp>
#include <CXO2/Messages/Responses/ChannelListResponse.hpp>
#include <CXO2/Messages/Requests/ChannelLoginRequest.hpp>
#include <CXO2/Messages/Responses/ChannelLoginResponse.hpp>

#include <functional>
#include <CXO2/Messages/MessageEnvelope.hpp>

namespace Cx
{
    class SessionContext;
    class MessageService;
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
        PlanetOnlineService(MessageService& messages, SessionContext& session);

        void GetChannelList(
            const MessageCallback<ChannelListResponse>& callback
        ) const override;

        void Login(
            const ChannelLoginRequest& request,
            const MessageCallback<ChannelLoginResponse>& callback
        ) const override;

    private:
        MessageService& m_messages;
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
