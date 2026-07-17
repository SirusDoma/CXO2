#pragma once

#include <CXO2/Network/MessageDispatcher.hpp>

#include <Genode/System/Module.hpp>
#include <Genode/Events/EventDispatcher.hpp>

#include <cstdint>
#include <functional>
#include <unordered_map>

namespace Cx
{
    template<
        class TMessage,
        typename = std::enable_if_t<IsMessage<TMessage>::value>
    >
    using MessageSubscriber = Gx::Events::Subscriber<std::uint16_t, MessageEnvelope<TMessage>>;

    class MessageService : public MessageDispatcher, public Gx::Module
    {
    public:
        MessageService(Gx::TcpNetworkClient& client, Gx::Events::EventDispatcher& events);

        void Connect(
            const sf::IpAddress& ipAddress,
            unsigned short port,
            const std::function<void()>& onConnected = nullptr,
            const std::function<void(const Gx::Exception&)>& onError = nullptr,
            sf::Time timeout = sf::Time::Zero
        );

        void Disconnect();

        template<class TMessage>
        std::enable_if_t<IsMessage<TMessage>::value, void>
        Dispatch(
            const TMessage& request,
            const std::function<void()>& onSuccess = nullptr,
            const std::function<void(const Gx::Exception&)>& onError = nullptr,
            sf::Time timeout = sf::Time::Zero
        );

        template<class TMessage>
        std::enable_if_t<IsMessage<TMessage>::value, void>
        Dispatch(
            const TMessage& request,
            const MessageCallback<TMessage>& callback = nullptr,
            sf::Time timeout = sf::Time::Zero
        );

        template<class TRequest, class TResponse>
        std::enable_if_t<IsMessage<TRequest>::value && IsMessage<TResponse>::value, void>
        Dispatch(
            const TRequest& request,
            const MessageCallback<TResponse>& callback = nullptr,
            sf::Time timeout = sf::Time::Zero
        );

        template<class TMessage>
        std::enable_if_t<IsMessage<TMessage>::value, void>
        Acquire(
            const MessageCallback<TMessage>& callback = nullptr,
            sf::Time timeout = sf::Time::Zero
        );

        template<class TMessage>
        [[nodiscard]] std::enable_if_t<IsMessage<TMessage>::value, MessageSubscriber<TMessage>>
        On(const MessageCallback<TMessage>& callback);

        template<class TRequest, class TResponse>
        std::enable_if_t<IsMessage<TRequest>::value && IsMessage<TResponse>::value, void>
        StartHeartbeat(
            sf::Time timeout,
            const std::function<void(const Gx::Exception&)>& errorCallback = nullptr
        );

        void StopHeartbeat();

        void Update(const sf::Time& delta) override;

    private:
        struct Subscription
        {
            std::size_t           Count{};
            bool                  Pending{false};
            std::function<void()> Acquire{};
        };

        struct Heartbeat
        {
            std::size_t           Generation{};
            bool                  Active{false};
            sf::Time              Elapsed{};
            sf::Time              Timeout{};
            std::function<void()> Beat{};
        };

        Gx::TcpNetworkClient&                           m_client;
        Gx::Events::EventDispatcher&                    m_events;
        std::unordered_map<std::uint16_t, Subscription> m_subscriptions{};
        Heartbeat                                       m_heartbeat{};
        std::function<void()>                           m_connectedCallback{};
        std::function<void(const Gx::Exception&)>       m_connectErrorCallback{};
        bool                                            m_connecting{false};
    };
}

#include <CXO2/Services/MessageService.inl>
