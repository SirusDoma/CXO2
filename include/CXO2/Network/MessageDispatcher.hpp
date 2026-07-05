#pragma once

#include <CXO2/Messages/Commands.hpp>
#include <CXO2/Messages/MessageEnvelope.hpp>
#include <CXO2/Network/Exception.hpp>
#include <CXO2/Network/NetworkClient.hpp>

#include <Genode/Entities/Updatable.hpp>

#include <cstdint>
#include <deque>
#include <exception>
#include <functional>
#include <unordered_map>

namespace Cx
{
    class MessageDispatcher : public Gx::Updatable
    {
    public:
        explicit MessageDispatcher(NetworkClient& client);

        void Update(const sf::Time& delta) override;

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
        std::enable_if_t<IsMessage<TMessage>::value, void>
        Discard();

    private:
        struct DispatchRequest
        {
            Request  Tracker{};
            sf::Time Elapsed{};
            sf::Time Timeout{};

            std::function<void()>                          OnCompleted{};
            std::function<void(const std::exception_ptr&)> OnError{};
        };

        struct AcquireRequest
        {
            std::uint16_t Code{};
            sf::Time      Elapsed{};
            sf::Time      Timeout{};

            std::function<void(Packet&)>                   OnMessage{};
            std::function<void(const std::exception_ptr&)> OnError{};
        };

        template<class TMessage>
        void Dispatch(
            const TMessage& request,
            std::function<void()> onCompleted,
            std::function<void(const std::exception_ptr&)> onError,
            sf::Time timeout
        );

        void ProcessDispatch(const sf::Time& delta);
        void ProcessAcquire(const sf::Time& delta);

        NetworkClient& m_client;

        std::deque<DispatchRequest> m_dispatches{};
        std::deque<AcquireRequest>  m_acquires{};

        std::unordered_map<std::uint16_t, std::deque<Packet>> m_messages{};
    };
}

#include <CXO2/Network/MessageDispatcher.inl>
