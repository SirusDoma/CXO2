#include <CXO2/Contexts/CommandLineContext.hpp>
#include <CXO2/Utilities/AuthTokenDecoder.hpp>

#include <Genode/Utilities/StringHelper.hpp>

#include <array>
#include <regex>

namespace Cx
{
    CommandLineContext::CommandLineContext(const int argc, char** argv)
    {
        for (std::size_t i = 1; i < argc; i++)
        {
            auto arg = std::string(argv[i]);
            m_arguments.push_back(arg);
        }
    }

    std::size_t CommandLineContext::GetArgumentCount() const
    {
        return m_arguments.size();
    }

    std::string CommandLineContext::GetArgument(const int index) const
    {
        if (index < 0 || index >= m_arguments.size())
            return std::string();

        return m_arguments[index];
    }

    std::string CommandLineContext::GetAuthToken() const
    {
        if (GetArgumentCount() < 1)
            return std::string();

        const auto token   = AuthTokenDecoder::Decode(m_arguments[0]);
        const auto pattern = std::regex(R"([0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12})");

        if (std::regex_match(token, pattern))
            return token;

        if (Gx::StringHelper::Split(token, ':').size() == 2)
            return token;

        return std::string();

    }

    std::string CommandLineContext::GetFtpUrl() const
    {
        if (GetArgumentCount() < 2)
            return std::string();

        return m_arguments[1];
    }

    std::string CommandLineContext::GetGame() const
    {
        if (GetArgumentCount() < 3)
            return std::string();

        return m_arguments[2];
    }

    std::vector<GatewayInfo> CommandLineContext::GetGatewayInfo() const
    {
        if (GetArgumentCount() < 6)
            return {};

        try
        {
            int gatewayCount = std::stoi(m_arguments[3]);

            // Different halls order for 3.10 compatibility
            auto gateways = std::vector<GatewayInfo>();
            auto halls = std::array<MusicHall, 6>
            {
                MusicHall::Melpomin,
                MusicHall::Kalliope,
                MusicHall::Kleo,
                MusicHall::Euta,
                MusicHall::Thalo,
                MusicHall::Philix,
            };

            for (std::size_t i = 0; i < gatewayCount; i++)
            {
                gateways.push_back({
                    halls[i],
                    sf::IpAddress::resolve(m_arguments[4 + i * 2]).value_or(sf::IpAddress::Any),
                    static_cast<std::uint16_t>(std::stoi(m_arguments[4 + i * 2 + 1]))
                });
            }

            return gateways;
        }
        catch (...)
        {
            return {};
        }
    }
}
