#pragma once

#include <SFML/System/Sleep.hpp>
#include <fmt/format.h>

template<typename T>
std::enable_if_t<std::is_integral_v<T>, void>
NetworkAdapter::UsePrefixSizeType()
{
    m_send = [](sf::TcpSocket& socket, Packet& packet, const sf::Time timeout) -> sf::Socket::Status
    {
        std::size_t payloadSize;
        std::size_t bytesSent;
        const sf::Clock timer{};

        const void* payloadData = packet.OnSend(payloadSize);
        
        const T packetSize = static_cast<T>(payloadSize + sizeof(T));
        std::vector<std::uint8_t> buffer(sizeof(T) + payloadSize);

        std::memcpy(buffer.data(), &packetSize, sizeof(T));
        if (payloadSize > 0)
            std::memcpy(buffer.data() + sizeof(T), payloadData, payloadSize);

        size_t offset = 0;
        while (offset < packetSize)
        {
            if (timeout != sf::Time::Zero && timer.getElapsedTime() > timeout)
                return sf::Socket::Status::Error;

            const auto status = socket.send(buffer.data() + offset, packetSize - offset, bytesSent);
            offset += bytesSent;

            if (status != sf::Socket::Status::Done && status != sf::Socket::Status::Partial)
                return status;
        }
        
        return sf::Socket::Status::Done;
    };

    m_receive = [](sf::TcpSocket& socket, Packet& packet, const sf::Time timeout) -> sf::Socket::Status
    {
        T prefix;
        std::size_t bytesReceived;
        sf::Socket::Status status;
        const sf::Clock timer{};

        size_t offset = 0;
        while (offset < sizeof(T))
        {
            if (timeout != sf::Time::Zero && timer.getElapsedTime() > timeout)
                return sf::Socket::Status::Error;

            status  = socket.receive(&prefix + offset, sizeof(T) - offset, bytesReceived);
            offset += bytesReceived;

            if (status != sf::Socket::Status::Done)
                return status;
        }

        if (static_cast<std::size_t>(prefix) == 0)
            return sf::Socket::Status::Done;

        const auto payloadSize = static_cast<std::size_t>(prefix) - sizeof(T);
        auto payloadBuffer     = std::vector<std::uint8_t>(payloadSize);

        offset = 0;
        while (offset < payloadSize)
        {
            if (timeout != sf::Time::Zero && timer.getElapsedTime() > timeout)
                return sf::Socket::Status::Error;

            status = socket.receive(payloadBuffer.data() + offset, payloadSize - offset, bytesReceived);
            offset += bytesReceived;

            if (status != sf::Socket::Status::Done && status != sf::Socket::Status::NotReady)
                return status;
        }

        packet.OnReceive(payloadBuffer.data(), payloadSize);
        return sf::Socket::Status::Done;
    };
}

template<class T>
std::enable_if_t<Packet::IsSerializable<T>::value, sf::Socket::Status>
NetworkAdapter::Send(const T& message, const sf::Time timeout)
{
    return Send<Packet, T>(message, timeout);
}

template<typename P, class T>
std::enable_if_t<
    std::is_base_of_v<Packet, P> &&
    std::is_default_constructible_v<P> &&
    Packet::IsSerializable<T>::value,
sf::Socket::Status>
NetworkAdapter::Send(const T& message, sf::Time timeout)
{
    auto packet = P();
    packet << T::Command;
    packet << message;

    return m_send(m_socket, packet, timeout);
}

template<class T>
std::enable_if_t<Packet::IsSerializable<T>::value, void>
NetworkAdapter::SendAsync(
    const T& message,
    std::function<void()> onSuccess,
    std::function<void(const NetworkException&)> onError,
    sf::Time timeout
)
{
    if (!IsStarted())
    {
        onError(NetworkException(Gx::InvalidOperationException("Connection lost")));
        return;
    }

    auto state = PumpRequest{};
    state.Command = T::Command;
    state.Request << T::Command;
    state.Request << message;
    state.Timeout = timeout;
    state.OnError = onError != nullptr ? onError : [] (const auto&) {};
    state.OnSuccess = [onSuccess]
    {
        if (onSuccess)
            onSuccess();
    };

    Enqueue(std::move(state));
}

template<class T>
std::enable_if_t<Packet::IsSerializable<T>::value, sf::Socket::Status>
NetworkAdapter::Receive(T& message, const sf::Time timeout)
{
    return Receive<Packet, T>(message, timeout);
}

template<typename P, class T>
std::enable_if_t<
    std::is_base_of_v<Packet, P> &&
    std::is_default_constructible_v<P> &&
    Packet::IsSerializable<T>::value,
sf::Socket::Status>
NetworkAdapter::Receive(T& message, sf::Time timeout)
{
    auto packet = P();
    auto status = m_receive(m_socket, packet, timeout);

    if (status != sf::Socket::Status::Done)
        return status;

    Command cmd;
    packet >> cmd;

    if (T::Command != cmd)
    {
        throw Gx::ArgumentException(fmt::format("Response command type mismatch. Expected: {:#04x} / Got: {:#04x}",
            T::Command.code(), cmd.code()));
    }

    packet >> message;
    return status;
}

template<class T>
std::enable_if_t<Packet::IsSerializable<T>::value,void>
NetworkAdapter::ReceiveAsync(
    std::function<void(const T&)> onSuccess,
    std::function<void(const NetworkException&)> onError,
    sf::Time timeout
)
{
    if (!IsStarted())
    {
        onError(NetworkException(Gx::InvalidOperationException("Connection lost")));
        return;
    }

    if (auto message = PollMessage<T>(); message.has_value())
    {
        if (onSuccess)
            onSuccess(message.value());

        return;
    }

    auto state = PollRequest{};
    state.Command   = T::Command;
    state.Timeout   = timeout;
    state.Recurring = false;
    state.OnError   = onError != nullptr ? onError : [] (const auto&) {};
    state.OnSuccess = [onSuccess, onError] (auto& packet)
    {
        try
        {
            T message{};
            packet >> message;

            if (onSuccess)
                onSuccess(message);
        }
        catch (NetworkException ex)
        {
            if (onError)
                onError(ex);
        }
        catch (std::runtime_error ex)
        {
            if (onError)
                onError(NetworkException(ex));
        }
    };

    Enqueue(std::move(state));
}

template<class T>
std::enable_if_t<Packet::IsSerializable<T>::value, void> NetworkAdapter::Subscribe(
    std::function<void(const T&)> onSuccess, std::function<void(const NetworkException&)> onError)
{
    if (!IsStarted())
    {
        onError(NetworkException(Gx::InvalidOperationException("Connection lost")));
        return;
    }

    auto state      = PollRequest{};
    state.Command   = T::Command;
    state.Timeout   = sf::Time::Zero;
    state.Recurring = true;
    state.OnError   = onError != nullptr ? onError : [] (const auto&) {};
    state.OnSuccess = [onSuccess, onError] (auto& packet)
    {
        try
        {
            T message{};
            packet >> message;

            if (onSuccess)
                onSuccess(message);
        }
        catch (NetworkException ex)
        {
            if (onError)
                onError(ex);
        }
        catch (std::runtime_error ex)
        {
            if (onError)
                onError(NetworkException(ex));
        }
    };

    Enqueue(std::move(state));
}

template<typename TMessage>
std::enable_if_t<IsMessage<TMessage>::value, std::optional<TMessage>> NetworkAdapter::PollMessage()
{
    if (!IsStarted())
        return std::nullopt;

    std::lock_guard lock(m_pollMutex);

    if (m_messages.empty())
        return std::nullopt;

    const auto commandCode = TMessage::Command.code();
    auto it = m_messages.find(commandCode);
    
    if (it == m_messages.end() || it->second.empty())
        return std::nullopt;
    
    // Get the first packet for this command
    auto packet = std::move(it->second.front());
    it->second.erase(it->second.begin());
    
    // If no more packets for this command, remove the entry
    if (it->second.empty())
        m_messages.erase(it);

    try
    {
        // Deserialize the message
        TMessage message{};
        packet >> message;

        return message;
    }
    catch (...)
    {
        // If deserialization fails, return nullopt
        return std::nullopt;
    }
}

template<typename TRequest, typename TResponse>
std::enable_if_t<
    IsMessage<TRequest>::value && IsMessage<TResponse>::value,
    void
>
NetworkAdapter::Exchange(
    const TRequest& request,
    std::function<void(const TResponse&)> onSuccess,
    std::function<void(const NetworkException&)> onError,
    const sf::Time timeout)
{
    if (!IsStarted())
    {
        onError(NetworkException(Gx::InvalidOperationException("Connection lost")));
        return;
    }

    auto pumpReq = PumpRequest{};
    pumpReq.Command = TRequest::Command;
    pumpReq.Request << TRequest::Command;
    pumpReq.Request << request;
    pumpReq.Timeout = timeout;
    pumpReq.OnError = onError != nullptr ? onError : [] (const auto&) {};

    auto pollReq = PollRequest{};
    pollReq.Command = TResponse::Command;
    pollReq.Timeout = timeout;
    pollReq.OnError = onError != nullptr ? onError : [] (const auto&) {};
    pollReq.OnSuccess = [onSuccess, onError] (Packet& packet)
    {
        try
        {
            TResponse message{};
            packet >> message;

            if (onSuccess)
                onSuccess(message);
        }
        catch (NetworkException ex)
        {
            if (onError)
                onError(ex);
        }
        catch (std::runtime_error ex)
        {
            if (onError)
                onError(NetworkException(ex));
        }
    };

    if (Enqueue(std::move(pumpReq)))
        Enqueue(std::move(pollReq));
}

template<typename TRequest, typename TResponse>
std::enable_if_t<IsMessage<TRequest>::value && IsMessage<TResponse>::value, void>
NetworkAdapter::StartHeartbeat(
    const sf::Time interval,
    std::function<void(const TResponse&)> callback,
    const std::function<void(const NetworkException&)> errorCallback
)
{
    if (!IsStarted())
        throw Gx::InvalidOperationException("Connection lost");

    m_heartbeat.Enabled  = false;
    if (m_heartbeat.Thread.joinable())
        m_heartbeat.Thread.join();

    m_heartbeat.ResponseCommand = TResponse::Command;
    m_heartbeat.Enabled  = true;
    m_heartbeat.Interval = interval;
    m_heartbeat.Thread   = std::thread([this, callback, errorCallback]
    {
        while (m_heartbeat.Enabled)
        {
            {
                auto lock = std::unique_lock(m_heartbeat.Mutex);
                if (m_heartbeat.Signal.wait_for(lock, m_heartbeat.Interval.toDuration(), [this] { return !m_heartbeat.Enabled; }))
                    break;
            }

            SendAsync<TRequest>(TRequest{}, nullptr, errorCallback);
            ReceiveAsync<TResponse>([this, callback] (const auto& response)
            {
                if (callback)
                    callback(response);

            }, errorCallback);
        }
    });
}