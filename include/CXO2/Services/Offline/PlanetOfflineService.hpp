#pragma once

#include <CXO2/Services/PlanetService.hpp>

namespace Cx
{
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
