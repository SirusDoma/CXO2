#pragma once

#include <Genode/System/Exception.hpp>

#include <SFML/Network/Socket.hpp>

#include <fmt/format.h>
#include <magic_enum/magic_enum.hpp>

class NetworkException : public Gx::Exception
{
public:
    explicit NetworkException(const std::runtime_error& ex) : Exception(ex.what()) {}

protected:
    explicit NetworkException(const std::string& message) : Exception(message) {}
};

class ConnectionException : public NetworkException
{
public:
    explicit ConnectionException(
        const sf::Socket::Status status)
    : NetworkException(fmt::format("Unexpected socket state: {}", magic_enum::enum_name(status))),
      m_status(status) {}

    explicit ConnectionException(
        const sf::Socket::Status status,
        const std::string& message)
    : NetworkException(message), m_status(status) {}

    sf::Socket::Status GetStatus() const { return m_status; }

private:
    sf::Socket::Status m_status;
};

class TimeoutException : public NetworkException
{
public:
    explicit TimeoutException(const std::string& message = "Operation has timed out.") : NetworkException(message) {}
};
