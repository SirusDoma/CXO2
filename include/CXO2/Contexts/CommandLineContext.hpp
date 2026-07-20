#pragma once
#include <CXO2/Models/Planet.hpp>
#include <SFML/Network/IpAddress.hpp>

#include <vector>

namespace Cx
{
    struct Gateway
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
        std::vector<Gateway> GetGatewayInfo() const;

    private:
        std::vector<std::string> m_arguments{};
    };
}
