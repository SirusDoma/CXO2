#pragma once

#include <CXO2/Services/Service.hpp>
#include <CXO2/Network/MessageEnvelope.hpp>
#include <CXO2/Network/Responses/ChannelListResponse.hpp>
#include <CXO2/Network/Requests/ChannelLoginRequest.hpp>
#include <CXO2/Network/Responses/ChannelLoginResponse.hpp>

#include <functional>

namespace Cx
{
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
}
