#include <CXO2/Network/MessageDispatcher.hpp>

namespace Cx
{
    MessageDispatcher::MessageDispatcher(Gx::TcpNetworkClient& client) :
        m_client(client)
    {
    }

    void MessageDispatcher::Update(const sf::Time& delta)
    {
        ProcessDispatch(delta);
        ProcessAcquire(delta);
    }

    void MessageDispatcher::ProcessDispatch(const sf::Time& delta)
    {
        for (std::size_t i = 0; i < m_dispatches.size();)
        {
            auto status    = Gx::RequestStatus::Pending;
            auto exception = std::exception_ptr();

            try
            {
                status = m_dispatches[i].Tracker->GetStatus();
            }
            catch (...)
            {
                exception = std::current_exception();
            }

            if (!exception && status == Gx::RequestStatus::Pending)
            {
                m_dispatches[i].Elapsed += delta;
                if (m_dispatches[i].Timeout == sf::Time::Zero || m_dispatches[i].Elapsed < m_dispatches[i].Timeout)
                {
                    ++i;
                    continue;
                }

                exception = std::make_exception_ptr(Gx::TimeoutException());
            }

            if (!exception && status == Gx::RequestStatus::Failed)
            {
                sf::Socket::Status connStatus{};

                try
                {
                    connStatus = m_client.GetStatus();
                }
                catch (...)
                {
                    connStatus = sf::Socket::Status::Error;
                    exception  = std::current_exception();
                }

                if (!exception)
                {
                    exception = std::make_exception_ptr(
                        Gx::ConnectionException(connStatus, "Failed to deliver the request to the server")
                    );
                }
            }

            auto entry = std::move(m_dispatches[i]);
            m_dispatches.erase(m_dispatches.begin() + i);

            if (exception)
                entry.OnError(exception);
            else
                entry.OnCompleted();
        }
    }

    void MessageDispatcher::ProcessAcquire(const sf::Time& delta)
    {
        try
        {
            for (auto& packet : m_client.Poll())
            {
                try
                {
                    Command code{};
                    packet >> code;

                    m_messages[code].push_back(std::move(packet));
                }
                catch (Gx::ArgumentException&)
                {
                    // Command Parse error, ignore the message completely
                }
            }
        }
        catch (...)
        {
            const auto exception = std::current_exception();

            auto acquires = std::deque<AcquireRequest>();
            acquires.swap(m_acquires);

            for (const auto& entry : acquires)
                entry.OnError(exception);
        }

        for (std::size_t i = 0; i < m_acquires.size();)
        {
            auto messages = m_messages.find(m_acquires[i].Code);
            if (messages == m_messages.end() || messages->second.empty())
            {
                m_acquires[i].Elapsed += delta;
                if (m_acquires[i].Timeout == sf::Time::Zero || m_acquires[i].Elapsed < m_acquires[i].Timeout)
                {
                    ++i;
                    continue;
                }

                auto entry = std::move(m_acquires[i]);
                m_acquires.erase(m_acquires.begin() + i);

                entry.OnError(std::make_exception_ptr(Gx::TimeoutException()));
                continue;
            }

            auto packet = std::move(messages->second.front());
            messages->second.pop_front();

            if (messages->second.empty())
                m_messages.erase(messages);

            auto entry = std::move(m_acquires[i]);
            m_acquires.erase(m_acquires.begin() + i);

            entry.OnMessage(packet);
        }
    }
}
