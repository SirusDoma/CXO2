#pragma once

#include <CXO2/Network/Packet.hpp>
#include <CXO2/Network/Exception.hpp>

#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/System/Time.hpp>

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <exception>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>

namespace Cx
{
    enum class RequestStatus
    {
        Pending,
        Completed,
        Failed,
        Cancelled
    };

    class RequestTracker final
    {
    public:
        RequestTracker() = default;

        explicit RequestTracker(const RequestStatus status, std::exception_ptr exception = nullptr) :
            m_status(status),
            m_exception(std::move(exception))
        {
        }

        RequestTracker& operator=(const RequestTracker& tracker)
        {
            m_exception = tracker.m_exception;
            m_status    = tracker.m_status.load();

            return *this;
        }

        [[nodiscard]] RequestStatus GetStatus() const
        {
            const auto status = m_status.load();
            if ((status == RequestStatus::Failed || status == RequestStatus::Cancelled) && m_exception)
                std::rethrow_exception(m_exception);

            return status;
        }

    private:
        std::atomic<RequestStatus> m_status{RequestStatus::Pending};
        std::exception_ptr         m_exception{};
    };

    using Request = std::shared_ptr<RequestTracker>;

    class NetworkClient final
    {
    public:
        NetworkClient() = default;
        ~NetworkClient();

        template<typename T>
        std::enable_if_t<std::is_unsigned_v<T> && !std::is_same_v<T, bool>, void>
        UseDefaultPrefix();

        void Connect(const sf::IpAddress& ipAddress, unsigned short port, sf::Time timeout = sf::Time::Zero);
        void Disconnect();

        [[nodiscard]] sf::Socket::Status GetStatus();

        Request Send(Packet packet);
        [[nodiscard]] std::vector<Packet> Poll();

    private:
        struct RequestEntry
        {
            Packet  Payload{};
            Request Result{};
        };

        enum class State
        {
            Stopped,
            Running,
            Stopping,
        };

        void Start();
        void Stop();

        void ProcessSend();
        void ProcessReceive();

        void Write(const void* data, std::size_t size);
        [[nodiscard]] bool Read(void* data, std::size_t size);

        sf::TcpSocket                   m_socket{};
        Packet::Prefix                  m_prefix{Packet::Prefix::Of<std::uint16_t>()};
        std::deque<RequestEntry>        m_requestQueue{};
        std::vector<Packet>             m_packets{};
        std::exception_ptr              m_connectException{};
        std::exception_ptr              m_receiveException{};
        std::mutex                      m_sendMutex{};
        std::mutex                      m_receiveMutex{};
        std::condition_variable         m_sendSignal{};
        std::thread                     m_connectThread{};
        std::thread                     m_sendThread{};
        std::thread                     m_receiveThread{};
        std::atomic<State>              m_state{State::Stopped};
        std::atomic<sf::Socket::Status> m_status{sf::Socket::Status::Disconnected};
    };
}

#include <CXO2/Network/NetworkClient.inl>
