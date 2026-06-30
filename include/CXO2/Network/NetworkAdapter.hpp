#pragma once

#include <CXO2/Network/Packet.hpp>
#include <CXO2/Messages/Commands.hpp>
#include <CXO2/Network/Exception.hpp>

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/System/Clock.hpp>

#include <functional>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <optional>
#include <condition_variable>

namespace Cx
{
    class NetworkAdapter
    {
    public:
        NetworkAdapter();
        NetworkAdapter(sf::IpAddress ipAddress, unsigned short port);
        ~NetworkAdapter();

        [[nodiscard]] sf::TcpSocket::Status Connect(sf::Time timeout = sf::Time::Zero);

        void Disconnect();

        template<typename T>
        std::enable_if_t<std::is_integral_v<T>, void>
        UsePrefixSizeType();

        bool IsStarted() const;

        template<typename TRequest, typename TResponse>
        std::enable_if_t<IsMessage<TRequest>::value && IsMessage<TResponse>::value, void>
        StartHeartbeat(
            sf::Time interval = sf::seconds(10),
            std::function<void(const TResponse&)> callback = nullptr,
            std::function<void(const NetworkException&)> errorCallback = nullptr
        );

        void StopHeartbeat();

        void Start(sf::Time timeout = sf::Time::Zero);
        void Start(sf::IpAddress ipAddress, unsigned short port, sf::Time timeout = sf::Time::Zero);
        void Stop();

        template<typename TMessage>
        [[nodiscard]] std::enable_if_t<IsMessage<TMessage>::value, std::optional<TMessage>>
        PollMessage();

        template<class T>
        [[nodiscard]] std::enable_if_t<Packet::IsSerializable<T>::value, sf::Socket::Status>
        Send(const T& message, sf::Time timeout = sf::Time::Zero);

        template<typename P, class T>
        [[nodiscard]] std::enable_if_t<
            std::is_base_of_v<Packet, P> &&
            std::is_default_constructible_v<P> &&
            Packet::IsSerializable<T>::value,
        sf::Socket::Status>
        Send(const T& message, sf::Time timeout = sf::Time::Zero);

        template<class T>
        std::enable_if_t<Packet::IsSerializable<T>::value,void>
        SendAsync(
            const T& message,
            std::function<void()> onSuccess = nullptr,
            std::function<void(const NetworkException&)> onError = nullptr,
            sf::Time timeout = sf::Time::Zero
        );

        template<class T>
        [[nodiscard]] std::enable_if_t<Packet::IsSerializable<T>::value, sf::Socket::Status>
        Receive(T& message, sf::Time timeout = sf::Time::Zero);

        template<typename P, class T>
        [[nodiscard]] std::enable_if_t<
            std::is_base_of_v<Packet, P> &&
            std::is_default_constructible_v<P> &&
            Packet::IsSerializable<T>::value,
        sf::Socket::Status>
        Receive(T& message, sf::Time timeout = sf::Time::Zero);

        template<class T>
        std::enable_if_t<Packet::IsSerializable<T>::value,void>
        ReceiveAsync(
            std::function<void(const T&)> onSuccess = nullptr,
            std::function<void(const NetworkException&)> onError = nullptr,
            sf::Time timeout = sf::Time::Zero
        );

        template<class T>
        std::enable_if_t<Packet::IsSerializable<T>::value,void>
        Subscribe(
            std::function<void(const T&)> onSuccess = nullptr,
            std::function<void(const NetworkException&)> onError = nullptr
        );

        template<typename TRequest, typename TResponse>
        std::enable_if_t<
            IsMessage<TRequest>::value && IsMessage<TResponse>::value,
            void
        >
        Exchange(
            const TRequest& request,
            std::function<void(const TResponse&)> onSuccess = nullptr,
            std::function<void(const NetworkException&)> onError = nullptr,
            sf::Time timeout = sf::seconds(15)
        );

        void Unsubscribe(Command command);

        void ResetQueue();

    private:
        struct PumpRequest
        {
            Cx::Command                                    Command{};
            Packet                                       Request{};
            sf::Clock                                    Timer{};
            sf::Time                                     Timeout{sf::seconds(5)};
            std::function<void()>                        OnSuccess{nullptr};
            std::function<void(const NetworkException&)> OnError{nullptr};
        };

        struct PollRequest
        {
            Cx::Command                                    Command{};
            sf::Clock                                    Timer{};
            sf::Time                                     Timeout{sf::seconds(5)};
            bool                                         Recurring{false};
            std::function<void(Packet&)>                 OnSuccess{nullptr};
            std::function<void(const NetworkException&)> OnError{nullptr};
        };

        struct HeartbeatState
        {
            bool                    Enabled{false};
            Command                 ResponseCommand{};
            sf::Clock               Timer{};
            sf::Time                Interval{sf::seconds(30)};
            std::condition_variable Signal{};
            std::mutex              Mutex{};
            std::thread             Thread{};
        };

        using PacketProcessor = std::function<sf::Socket::Status(sf::TcpSocket&, Packet&, sf::Time)>;
        using MessageMap      = std::unordered_map<std::uint16_t, std::vector<Packet>>;

        bool Enqueue(const PumpRequest& request);
        bool Enqueue(const PollRequest& request);

        void Pump();
        void Poll();

        sf::IpAddress m_address{sf::IpAddress::Any};
        unsigned short m_port{};

        sf::TcpSocket   m_socket{};
        PacketProcessor m_send{};
        PacketProcessor m_receive{};
        HeartbeatState  m_heartbeat{};
        std::thread     m_poll{}, m_pump{};
        bool            m_started{false};

        MessageMap               m_messages{};
        std::mutex               m_pumpMutex{}, m_pollMutex{};
        std::condition_variable  m_signal{};

        std::vector<PumpRequest> m_pumpQueue{};
        std::vector<PollRequest> m_pollQueue{};
    };

}

#include <CXO2/Network/NetworkAdapter.inl>
