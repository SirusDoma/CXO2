#pragma once

#include <OTwo/Services/Service.hpp>
#include <OTwo/Messages/Responses/ChannelListResponse.hpp>
#include <OTwo/Messages/Requests/ChannelLoginRequest.hpp>
#include <OTwo/Messages/Responses/ChannelLoginResponse.hpp>

#include <functional>

class SessionContext;
class NetworkAdapter;
class NetworkException;
class PlanetService : public Service
{
public:
    virtual void GetChannelList(
        std::function<void(const ChannelListResponse&)> callback,
        std::function<void(const NetworkException&)> onError = nullptr
    ) const = 0;

    virtual void Login(
        const ChannelLoginRequest& request,
        std::function<void(const ChannelLoginResponse&)> callback,
        std::function<void(const NetworkException&)> onError = nullptr
    ) const = 0;
};

class PlanetOnlineService : public PlanetService
{
public:
    PlanetOnlineService(NetworkAdapter& adapter, SessionContext& session);

    void GetChannelList(
        std::function<void(const ChannelListResponse&)> callback,
        std::function<void(const NetworkException&)> errorCallback = nullptr
    ) const override;

    void Login(
        const ChannelLoginRequest& request,
        std::function<void(const ChannelLoginResponse&)> callback,
        std::function<void(const NetworkException&)> errorCallback = nullptr
    ) const override;

private:
    NetworkAdapter& m_adapter;
    SessionContext& m_session;
};

class PlanetOfflineService : public PlanetService
{
public:
    void GetChannelList(
        std::function<void(const ChannelListResponse&)> callback,
        std::function<void(const NetworkException&)> onError = nullptr
    ) const override;

    void Login(
        const ChannelLoginRequest& request,
        std::function<void(const ChannelLoginResponse&)> callback,
        std::function<void(const NetworkException&)> onError = nullptr
    ) const override;
};


