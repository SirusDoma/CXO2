#include <CXO2/Network/NetworkClient.hpp>

#include <fmt/format.h>

#include <array>
#include <thread>
#include <vector>

namespace Cx
{
    NetworkClient::~NetworkClient()
    {
        Disconnect();
    }

    void NetworkClient::Connect(const sf::IpAddress& ipAddress, const unsigned short port, const sf::Time timeout)
    {
        Disconnect();

        m_status = sf::Socket::Status::NotReady;
        m_connectThread = std::thread([this, ipAddress, port, timeout]
        {
            const auto status = m_socket.connect(ipAddress, port, timeout);
            if (m_state == State::Stopping)
            {
                m_socket.disconnect();
                m_status = sf::Socket::Status::Disconnected;

                return;
            }

            if (status == sf::Socket::Status::Error)
            {
                m_status = sf::Socket::Status::Error;
                m_connectException = std::make_exception_ptr(
                    ConnectionException(status, "Failed to connect to the server"));

                return;
            }

            if (status == sf::Socket::Status::NotReady)
            {
                m_socket.disconnect();
                m_status = sf::Socket::Status::Error;
                m_connectException = std::make_exception_ptr(
                    TimeoutException("Connection attempt has timed out"));

                return;
            }

            m_status = status;
            if (status == sf::Socket::Status::Done)
                Start();
        });
    }

    void NetworkClient::Disconnect()
    {
        {
            auto lock = std::lock_guard(m_sendMutex);
            m_state = State::Stopping;
        }

        m_sendSignal.notify_all();
        m_socket.disconnect();

        if (m_connectThread.joinable())
            m_connectThread.join();

        if (m_sendThread.joinable())
            m_sendThread.join();

        if (m_receiveThread.joinable())
            m_receiveThread.join();

        {
            auto lock = std::lock_guard(m_receiveMutex);
            m_packets.clear();
            m_receiveException = nullptr;
        }

        m_connectException = nullptr;
        m_state  = State::Stopped;
        m_status = sf::Socket::Status::Disconnected;
    }

    sf::Socket::Status NetworkClient::GetStatus()
    {
        auto status = m_status.load();
        if (status == sf::Socket::Status::Error && m_connectException)
        {
            const auto exception = m_connectException;
            m_connectException   = nullptr;

            std::rethrow_exception(exception);
        }

        if (status == sf::Socket::Status::Done && m_socket.getRemotePort() == 0)
        {
            status   = sf::Socket::Status::Disconnected;
            m_status = status;
        }

        return status;
    }

    Request NetworkClient::Send(Packet packet)
    {
        auto state = std::make_shared<RequestTracker>();

        {
            auto lock = std::lock_guard(m_sendMutex);
            if (m_state == State::Running)
            {
                auto request    = RequestEntry{};
                request.Payload = std::move(packet);
                request.Result  = state;

                m_requestQueue.push_back(std::move(request));
                m_sendSignal.notify_one();

                return state;
            }
        }

        *state = RequestTracker(RequestStatus::Cancelled);
        return state;
    }

    std::vector<Packet> NetworkClient::Poll()
    {
        auto packets = std::vector<Packet>();

        {
            auto lock = std::lock_guard(m_receiveMutex);
            if (m_receiveException)
            {
                const auto exception = m_receiveException;
                m_receiveException   = nullptr;

                std::rethrow_exception(exception);
            }

            packets.swap(m_packets);
        }

        return packets;
    }

    void NetworkClient::Start()
    {
        {
            auto lock = std::lock_guard(m_sendMutex);
            m_state = State::Running;
        }

        m_sendThread    = std::thread(&NetworkClient::ProcessSend, this);
        m_receiveThread = std::thread(&NetworkClient::ProcessReceive, this);
    }

    void NetworkClient::Stop()
    {
        {
            auto lock = std::lock_guard(m_sendMutex);
            if (m_state != State::Running)
                return;

            m_state = State::Stopped;
        }

        m_sendSignal.notify_all();
        m_socket.disconnect();
        m_status = sf::Socket::Status::Disconnected;
    }

    void NetworkClient::ProcessSend()
    {
        while (true)
        {
            auto request = RequestEntry{};

            {
                auto lock = std::unique_lock(m_sendMutex);
                m_sendSignal.wait(lock, [this] { return !m_requestQueue.empty() || m_state != State::Running; });

                if (m_state != State::Running)
                    break;

                request = std::move(m_requestQueue.front());
                m_requestQueue.pop_front();
            }

            try
            {
                auto&       payload = request.Payload;
                const auto& prefix  = payload.m_prefix ? payload.m_prefix : m_prefix;

                std::size_t payloadSize = 0;
                const auto* payloadData = payload.OnSend(payloadSize);

                const auto prefixSize = prefix.GetSize();
                const auto packetSize = prefixSize + payloadSize;

                if (packetSize > prefix.GetMaxValue())
                {
                    throw ProtocolException(fmt::format(
                        "Packet size ({} bytes) exceeds the maximum value representable by the prefix size type ({} bytes)",
                        packetSize, prefix.GetMaxValue()));
                }

                auto prefixBuffer = std::array<std::byte, sizeof(std::uint64_t)>{};
                prefix.Encode(prefixBuffer.data(), packetSize);

                Write(prefixBuffer.data(), prefixSize);
                if (payloadSize > 0)
                    Write(payloadData, payloadSize);
            }
            catch (const ConnectionException& ex)
            {
                *request.Result = RequestTracker(RequestStatus::Failed, std::current_exception());

                if (m_state == State::Running)
                    m_status = ex.GetStatus();

                if (ex.GetStatus() == sf::Socket::Status::Disconnected)
                    break;

                continue;
            }
            catch (...)
            {
                *request.Result = RequestTracker(RequestStatus::Failed, std::current_exception());
                continue;
            }

            *request.Result = RequestTracker(RequestStatus::Completed);
        }

        Stop();

        auto pending = std::deque<RequestEntry>();

        {
            auto lock = std::lock_guard(m_sendMutex);
            pending.swap(m_requestQueue);
        }

        for (const auto& request : pending)
            *request.Result = RequestTracker(RequestStatus::Cancelled);
    }

    void NetworkClient::ProcessReceive()
    {
        while (true)
        {
            try
            {
                const auto prefixSize = m_prefix.GetSize();

                auto prefixBuffer = std::array<std::uint8_t, sizeof(std::uint64_t)>{};
                if (!Read(prefixBuffer.data(), prefixSize))
                    break;

                const auto packetSize = m_prefix.Decode(prefixBuffer.data());
                if (packetSize < prefixSize)
                    throw ProtocolException(fmt::format("Malformed packet size prefix: {}", packetSize));

                auto packet = Packet();

                const auto payloadSize = packetSize - prefixSize;
                if (payloadSize > 0)
                {
                    auto payload = std::vector<std::byte>(payloadSize);
                    if (!Read(payload.data(), payloadSize))
                        break;

                    packet.OnReceive(payload.data(), payloadSize);
                }

                {
                    auto lock = std::lock_guard(m_receiveMutex);
                    m_packets.push_back(std::move(packet));
                }
            }
            catch (...)
            {
                if (m_state == State::Running)
                {
                    auto lock = std::lock_guard(m_receiveMutex);
                    m_receiveException = std::current_exception();
                }

                break;
            }
        }

        Stop();
    }

    void NetworkClient::Write(const void* data, const std::size_t size)
    {
        const auto* buffer = static_cast<const std::byte*>(data);

        std::size_t offset = 0;
        while (offset < size)
        {
            std::size_t sent  = 0;
            const auto status = m_socket.send(buffer + offset, size - offset, sent);
            offset += sent;

            if (status != sf::Socket::Status::Done && status != sf::Socket::Status::Partial)
                throw ConnectionException(status, "Failed to send data to the server");
        }
    }

    bool NetworkClient::Read(void* data, const std::size_t size)
    {
        auto* buffer = static_cast<std::byte*>(data);

        std::size_t offset = 0;
        while (offset < size)
        {
            std::size_t received = 0;
            const auto status    = m_socket.receive(buffer + offset, size - offset, received);
            offset += received;

            if (status == sf::Socket::Status::Disconnected)
                return false;

            if (status != sf::Socket::Status::Done && status != sf::Socket::Status::Partial)
                throw ConnectionException(status, "Failed to receive data from the server");
        }

        return true;
    }
}
