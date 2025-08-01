#pragma once
#include <unordered_map>
#include <OTwo/Models/Planet.hpp>
#include <SFML/Network/IpAddress.hpp>

struct GatewayInfo
{
    MusicHall     Hall;
    sf::IpAddress Address;
    std::uint16_t Port;
};

class CommandLineContext
{
public:
    CommandLineContext() = default;
    CommandLineContext(int argc , char** argv);

    std::size_t GetArgumentCount() const;
    std::string GetArgument(int index) const;

    std::string GetAuthToken() const;
    std::string GetFtpUrl() const;
    std::string GetGame() const;
    std::vector<GatewayInfo> GetGatewayInfo() const;

private:
    std::vector<std::string> m_arguments{};
};
