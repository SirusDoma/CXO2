#include <CXO2/Services/MessageService.hpp>

namespace Cx
{
    MessageService::MessageService(Gx::TcpNetworkClient& client, Gx::Events::EventDispatcher& events) :
        MessageDispatcher(client),
        m_client(client),
        m_events(events)
    {
    }

    void MessageService::Connect(const sf::IpAddress& ipAddress, const unsigned short port,
        const std::function<void()>& onConnected,
        const std::function<void(const Gx::Exception&)>& onError,
        const sf::Time timeout)
    {
        m_connectedCallback    = onConnected;
        m_connectErrorCallback = onError;
        m_connecting           = true;

        m_client.Connect(ipAddress, port, timeout);
    }

    void MessageService::Disconnect()
    {
        m_connecting           = false;
        m_connectedCallback    = nullptr;
        m_connectErrorCallback = nullptr;

        m_client.Disconnect();
    }

    void MessageService::StopHeartbeat()
    {
        ++m_heartbeat.Generation;

        m_heartbeat.Active  = false;
        m_heartbeat.Elapsed = sf::Time::Zero;
        m_heartbeat.Beat    = nullptr;
    }

    void MessageService::Update(const sf::Time& delta)
    {
        if (m_connecting)
        {
            auto status    = sf::Socket::Status::NotReady;
            auto exception = std::exception_ptr();

            try
            {
                status = m_client.GetStatus();
            }
            catch (...)
            {
                exception = std::current_exception();
            }

            if (exception || status != sf::Socket::Status::NotReady)
            {
                const auto onConnected = m_connectedCallback;
                const auto onError     = m_connectErrorCallback;

                m_connecting           = false;
                m_connectedCallback    = nullptr;
                m_connectErrorCallback = nullptr;

                if (!exception && status != sf::Socket::Status::Done)
                {
                    exception = std::make_exception_ptr(
                        Gx::ConnectionException(status, "Failed to connect to the server"));
                }

                if (!exception)
                {
                    if (onConnected)
                        onConnected();
                }
                else if (onError)
                {
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
                }
            }
        }

        if (m_heartbeat.Active)
        {
            m_heartbeat.Elapsed += delta;
            if (m_heartbeat.Elapsed >= m_heartbeat.Timeout)
            {
                m_heartbeat.Elapsed = sf::Time::Zero;
                m_heartbeat.Beat();
            }
        }

        for (auto& [code, subscription] : m_subscriptions)
        {
            if (!subscription.Pending)
            {
                subscription.Pending = true;
                subscription.Acquire();
            }
        }

        MessageDispatcher::Update(delta);
    }
}
