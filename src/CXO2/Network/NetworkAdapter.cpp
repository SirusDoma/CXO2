#include <CXO2/Network/NetworkAdapter.hpp>

#include <Genode/System/Exception.hpp>
#include <SFML/Network/IpAddress.hpp>

#include <thread>

namespace Cx
{
    NetworkAdapter::NetworkAdapter() :
        m_address(sf::IpAddress::Any),
        m_port(),
        m_socket(),
        m_send(),
        m_receive()
    {
        UsePrefixSizeType<std::uint16_t>();
    }

    NetworkAdapter::NetworkAdapter(const sf::IpAddress ipAddress, const unsigned short port) :
        m_address(ipAddress),
        m_port(port),
        m_socket(),
        m_send(),
        m_receive()
    {
        UsePrefixSizeType<std::uint16_t>();
    }

    NetworkAdapter::~NetworkAdapter()
    {
        if (m_started)
            Stop();

        if (m_poll.joinable())
            m_poll.join();

        if (m_pump.joinable())
            m_pump.join();
    }

    sf::TcpSocket::Status NetworkAdapter::Connect(const sf::Time timeout)
    {
        Disconnect();

        m_socket.setBlocking(true);
        const auto status = m_socket.connect(m_address, m_port, timeout);

        m_socket.setBlocking(false);
        return status;
    }

    void NetworkAdapter::Disconnect()
    {
        m_socket.disconnect();
    }

    void NetworkAdapter::Start(const sf::IpAddress ipAddress, const unsigned short port, const sf::Time timeout)
    {
        if (m_started)
            return;

        m_address = ipAddress;
        m_port    = port;

        Start(timeout);
    }

    bool NetworkAdapter::IsStarted() const
    {
        return m_started;
    }

    void NetworkAdapter::StopHeartbeat()
    {
        m_heartbeat.Enabled = false;
        m_heartbeat.Signal.notify_one();
        if (m_heartbeat.Thread.joinable())
            m_heartbeat.Thread.join();
    }

    void NetworkAdapter::Start(const sf::Time timeout)
    {
        if (m_started)
            return;

        if (const auto status = Connect(timeout); status != sf::Socket::Status::Done)
            throw ConnectionException(status, "Failed to connect to the server");

        if (m_poll.joinable())
            m_poll.join();

        if (m_pump.joinable())
            m_pump.join();

        m_started = true;
        m_pump = std::thread(&NetworkAdapter::Pump, this);
        m_poll = std::thread(&NetworkAdapter::Poll, this);
    }

    bool NetworkAdapter::Enqueue(const PumpRequest& request)
    {
        auto lock = std::lock_guard(m_pumpMutex);
        for (const auto& pending : m_pumpQueue)
        {
            if (pending.Command.code() == request.Command.code())
                return false;
        }

        m_pumpQueue.push_back(request);
        m_signal.notify_one();
        return true;
    }

    bool NetworkAdapter::Enqueue(const PollRequest& request)
    {
        auto lock = std::lock_guard(m_pollMutex);
        m_pollQueue.push_back(request);
        return true;
    }

    void NetworkAdapter::Pump()
    {
        while (IsStarted())
        {
            auto tasks = std::vector<std::function<void()>>();

            {
                auto lock = std::unique_lock(m_pumpMutex);
                m_signal.wait(lock, [this] { return !m_pumpQueue.empty() || !IsStarted(); });

                if (!IsStarted())
                    break;

                for (auto it = m_pumpQueue.begin(); it != m_pumpQueue.end();)
                {
                    auto& state = *it;
                    const auto status = m_send(m_socket, state.Request, state.Timeout);

                    if (status != sf::Socket::Status::Done)
                    {
                        // TODO: Use custom send status to distinguish actual "Error" and timeout
                        tasks.push_back([state, status]
                        {
                            if (state.OnError)
                                state.OnError(ConnectionException(status, "Failed to connect to the server"));
                        });

                        it = m_pumpQueue.erase(it);
                        continue;
                    }

                    tasks.push_back([state]
                    {
                        if (state.OnSuccess)
                            state.OnSuccess();
                    });

                    it = m_pumpQueue.erase(it);
                }
            }

            for (auto& task : tasks)
                task();
        }
    }

    void NetworkAdapter::Poll()
    {
        while (IsStarted())
        {
            // Use delayed processing: The success (or error) handler may trigger another `Exchange` request
            //                         In which it may cause deadlock if the handler triggered directly
            auto tasks = std::vector<std::function<void()>>();
            {
                std::lock_guard lock(m_pollMutex);
                for (auto it = m_pollQueue.begin(); it != m_pollQueue.end();)
                {
                    auto state = *it;
                    if (const auto m = m_messages.find(state.Command.code()); m != m_messages.end() && !m->second.empty())
                    {
                        auto p = m->second.front();
                        tasks.push_back([state, p]
                        {
                            if (state.OnSuccess)
                            {
                                auto packet = std::move(p);
                                state.OnSuccess(packet);
                            }
                        });

                        m->second.erase(m->second.begin());
                        if (!state.Recurring)
                            it = m_pollQueue.erase(it);
                        else
                            ++it;
                    }
                    else if (!state.Recurring && state.Timeout != sf::Time::Zero && state.Timer.getElapsedTime() >= state.Timeout)
                    {
                        tasks.push_back([state] { state.OnError(TimeoutException()); });
                        it = m_pollQueue.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
            }

            for (auto& task : tasks)
                task();

            tasks.clear();

            Packet packet;
            Command command{};

            const auto status = m_receive(m_socket, packet, sf::Time::Zero);
            if (packet.GetDataSize() > 0 && status == sf::Socket::Status::Done)
            {
                try
                {
                    packet >> command;
                    {
                        std::lock_guard lock(m_pollMutex);

                        bool shouldEnqueue = true;
                        for (auto it = m_pollQueue.begin(); it != m_pollQueue.end();)
                        {
                            auto state = *it;
                            if (state.Command.code() == command.code())
                            {
                                shouldEnqueue = false;
                                tasks.push_back([state, &packet]
                                {
                                    if (state.OnSuccess)
                                        state.OnSuccess(packet);
                                });

                                if (!state.Recurring)
                                    it = m_pollQueue.erase(it);
                                else
                                    ++it;
                            }
                            else
                            {
                                ++it;
                            }
                        }

                        if (shouldEnqueue)
                            m_messages[command.code()].push_back(packet);
                    }

                    for (auto& task : tasks)
                        task();
                }
                catch (std::runtime_error ex)
                {
                    std::lock_guard lock(m_pollMutex);

                    // If command parsing fails, discard the packet
                    if (!m_pollQueue.empty())
                    {
                        if (auto& state = m_pollQueue.front(); state.Command == command)
                        {
                            if (state.OnError)
                                state.OnError(NetworkException(ex));

                            m_pollQueue.erase(m_pollQueue.begin());
                        }
                    }
                }
            }
            else if (status == sf::Socket::Status::Disconnected ||
                     status == sf::Socket::Status::Error)
            {
                std::lock_guard lock(m_pollMutex);

                // Connection loss, flush last request in the queue and stop processing
                while (!m_pollQueue.empty())
                {
                    if (const auto& state = m_pollQueue.front(); state.OnError)
                        state.OnError(ConnectionException(status, "Connection lost"));

                    m_pollQueue.erase(m_pollQueue.begin());
                }

                break;
            }
        }

        Stop();
    }

    void NetworkAdapter::Unsubscribe(const Command command)
    {
        std::lock_guard lock(m_pollMutex);

        for (auto it = m_pollQueue.begin(); it != m_pollQueue.end();)
        {
            const auto& state = *it;
            if (state.Recurring && state.Command.code() == command.code())
                it = m_pollQueue.erase(it);
            else
                ++it;
        }
    }

    void NetworkAdapter::Stop()
    {
        if (!m_started)
            return;

        m_started = false;
        m_signal.notify_one();

        Disconnect();
        StopHeartbeat();
        ResetQueue();
    }

    void NetworkAdapter::ResetQueue()
    {
        std::lock_guard lock1(m_pumpMutex);
        std::lock_guard lock2(m_pollMutex);

        m_messages.clear();
        m_pumpQueue.clear();
        m_pollQueue.clear();
    }
}
