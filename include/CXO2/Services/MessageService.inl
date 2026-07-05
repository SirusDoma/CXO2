#pragma once

namespace Cx
{
    template<class TMessage>
    std::enable_if_t<IsMessage<TMessage>::value, void>
    MessageService::Dispatch(const TMessage& request,
        const std::function<void()>& onSuccess,
        const std::function<void(const Gx::Exception&)>& onError,
        const sf::Time timeout)
    {
        auto envelope = MessageEnvelope<TMessage>(request);
        m_events.Dispatch(TMessage::Command.code(), envelope);

        try
        {
            MessageDispatcher::Dispatch(envelope.Open(), onSuccess, onError, timeout);
        }
        catch (const Gx::Exception& ex)
        {
            if (onError)
                onError(ex);
        }
        catch (const std::exception& ex)
        {
            if (onError)
                onError(Gx::Exception(ex.what()));
        }
        catch (...)
        {
            if (onError)
                onError(Gx::Exception("An unknown error has occurred"));
        }
    }

    template<class TMessage>
    std::enable_if_t<IsMessage<TMessage>::value, void> MessageService::Dispatch(
        const TMessage& request,
        const MessageCallback<TMessage>& callback,
        const sf::Time timeout)
    {
        auto envelope = MessageEnvelope<TMessage>(request);
        m_events.Dispatch(TMessage::Command.code(), envelope);

        try
        {
            MessageDispatcher::Dispatch(envelope.Open(), callback, timeout);
        }
        catch (...)
        {
            if (callback)
                callback(MessageEnvelope<TMessage>(std::current_exception()));
        }
    }

    template<class TRequest, class TResponse>
    std::enable_if_t<IsMessage<TRequest>::value && IsMessage<TResponse>::value, void>
    MessageService::Dispatch(const TRequest& request,
        const MessageCallback<TResponse>& callback,
        const sf::Time timeout)
    {
        auto envelope = MessageEnvelope<TRequest>(request);
        m_events.Dispatch(TRequest::Command.code(), envelope);

        try
        {
            MessageDispatcher::Dispatch<TRequest, TResponse>(
                envelope.Open(),
                [this, callback](const MessageEnvelope<TResponse>& response)
                {
                    auto result = response;
                    m_events.Dispatch(TResponse::Command.code(), result);

                    if (callback)
                        callback(result);
                },
                timeout
            );
        }
        catch (...)
        {
            if (callback)
                callback(MessageEnvelope<TResponse>(std::current_exception()));
        }
    }

    template<class TMessage>
    std::enable_if_t<IsMessage<TMessage>::value, void>
    MessageService::Acquire(const MessageCallback<TMessage>& callback, const sf::Time timeout)
    {
        MessageDispatcher::Acquire<TMessage>(
            [this, callback](const MessageEnvelope<TMessage>& envelope)
            {
                auto result = envelope;
                m_events.Dispatch(TMessage::Command.code(), result);

                if (callback)
                    callback(result);
            },
            timeout
        );
    }

    template<class TMessage>
    std::enable_if_t<IsMessage<TMessage>::value, MessageSubscriber<TMessage>>
    MessageService::On(const MessageCallback<TMessage>& callback)
    {
        auto subscriber = m_events.On(TMessage::Command.code(),
            [callback](const MessageEnvelope<TMessage>& envelope)
            {
                if (callback)
                    callback(envelope);
            },
            [this]
            {
                const auto it = m_subscriptions.find(TMessage::Command.code());
                if (it != m_subscriptions.end() && --it->second.Count == 0)
                    m_subscriptions.erase(it);
            });

        auto& subscription = m_subscriptions[TMessage::Command.code()];
        if (!subscription.Acquire)
        {
            subscription.Acquire = [this]
            {
                MessageDispatcher::Acquire<TMessage>(
                    [this](const MessageEnvelope<TMessage>& envelope)
                    {
                        const auto it = m_subscriptions.find(TMessage::Command.code());
                        if (it != m_subscriptions.end())
                            it->second.Pending = false;

                        auto result = envelope;
                        m_events.Dispatch(TMessage::Command.code(), result);
                    },
                    sf::Time::Zero
                );
            };
        }

        ++subscription.Count;
        return subscriber;
    }

    template<class TRequest, class TResponse>
    std::enable_if_t<IsMessage<TRequest>::value && IsMessage<TResponse>::value, void>
    MessageService::StartHeartbeat(const sf::Time timeout, const std::function<void(const Gx::Exception&)>& errorCallback)
    {
        Discard<TResponse>();

        const auto generation = ++m_heartbeat.Generation;

        m_heartbeat.Active  = true;
        m_heartbeat.Elapsed = timeout;
        m_heartbeat.Timeout = timeout;
        m_heartbeat.Beat    = [this, generation, timeout, errorCallback]
        {
            Dispatch<TRequest, TResponse>(
                TRequest{},
                [this, generation, errorCallback](const MessageEnvelope<TResponse>& envelope)
                {
                    if (!m_heartbeat.Active || m_heartbeat.Generation != generation)
                        return;

                    try
                    {
                        const auto& _ = envelope.Open();
                    }
                    catch (const Gx::Exception& ex)
                    {
                        if (errorCallback)
                            errorCallback(ex);
                    }
                    catch (const std::exception& ex)
                    {
                        if (errorCallback)
                            errorCallback(Gx::Exception(ex.what()));
                    }
                    catch (...)
                    {
                        if (errorCallback)
                            errorCallback(Gx::Exception("An unknown error has occurred"));
                    }
                },
                timeout
            );
        };
    }
}
