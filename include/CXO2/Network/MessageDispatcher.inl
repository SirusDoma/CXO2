#pragma once

namespace Cx
{
    template<class TMessage>
    std::enable_if_t<IsMessage<TMessage>::value, void>
    MessageDispatcher::Dispatch(const TMessage& request,
        const std::function<void()>& onSuccess,
        const std::function<void(const Gx::Exception&)>& onError,
        const sf::Time timeout)
    {
        Dispatch(
            request,
            [onSuccess]
            {
                if (onSuccess)
                    onSuccess();
            },
            [onError] (const std::exception_ptr& exception)
            {
                if (!onError)
                    return;

                try
                {
                    std::rethrow_exception(exception);
                }
                catch (const Gx::Exception& ex)
                {
                    onError(ex);
                }
                catch (const std::exception& ex)
                {
                    onError(Gx::Exception(ex.what()));
                }
                catch (...)
                {
                    onError(Gx::Exception("An unknown error has occurred"));
                }
            },
            timeout
        );
    }

    template<class TMessage>
    std::enable_if_t<IsMessage<TMessage>::value, void> MessageDispatcher::Dispatch(
        const TMessage& request,
        const MessageCallback<TMessage>& callback,
        const sf::Time timeout)
    {
        Dispatch(
            request,
            [callback, request]
            {
                if (callback)
                    callback(request);
            },
            [callback] (const std::exception_ptr& exception)
            {
                if (callback)
                    callback(MessageEnvelope<TMessage>(exception));
            },
            timeout
        );
    }

    template<class TRequest, class TResponse>
    std::enable_if_t<IsMessage<TRequest>::value && IsMessage<TResponse>::value, void>
    MessageDispatcher::Dispatch(const TRequest& request,
        const MessageCallback<TResponse>& callback,
        const sf::Time timeout)
    {
        Dispatch(
            request,
            [this, callback, timeout]
            {
                Acquire<TResponse>(callback, timeout);
            },
            [callback](const std::exception_ptr& exception)
            {
                if (callback)
                    callback(MessageEnvelope<TResponse>(exception));
            },
            timeout
        );
    }

    template<class TMessage>
    std::enable_if_t<IsMessage<TMessage>::value, void>
    MessageDispatcher::Acquire(const MessageCallback<TMessage>& callback, const sf::Time timeout)
    {
        auto request    = AcquireRequest{};
        request.Code    = TMessage::Command.code();
        request.Timeout = timeout;

        request.OnMessage = [callback](Gx::Packet& packet)
        {
            auto envelope = MessageEnvelope<TMessage>();

            try
            {
                auto message = TMessage();
                packet >> message;

                envelope = MessageEnvelope<TMessage>(std::move(message));
            }
            catch (...)
            {
                envelope = MessageEnvelope<TMessage>(std::current_exception());
            }

            if (callback)
                callback(envelope);
        };

        request.OnError = [callback](const std::exception_ptr& exception)
        {
            if (callback)
                callback(MessageEnvelope<TMessage>(exception));
        };

        m_acquires.push_back(std::move(request));
    }

    template<class TMessage>
    std::enable_if_t<IsMessage<TMessage>::value, void>
    MessageDispatcher::Discard()
    {
        m_messages.erase(TMessage::Command.code());
    }

    template<class TMessage>
    void MessageDispatcher::Dispatch(const TMessage& request,
        std::function<void()> onCompleted,
        std::function<void(const std::exception_ptr&)> onError,
        const sf::Time timeout)
    {
        auto packet = Gx::Packet();

        try
        {
            packet << TMessage::Command.code() << request;
        }
        catch (...)
        {
            onError(std::current_exception());
            return;
        }

        auto entry        = DispatchRequest{};
        entry.Tracker     = m_client.Send(std::move(packet));
        entry.Timeout     = timeout;
        entry.OnCompleted = std::move(onCompleted);
        entry.OnError     = std::move(onError);

        m_dispatches.push_back(std::move(entry));
    }
}
