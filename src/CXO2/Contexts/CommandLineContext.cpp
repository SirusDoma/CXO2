#include <CXO2/Contexts/CommandLineContext.hpp>
#include <CXO2/Utilities/AuthTokenDecoder.hpp>

#include <Genode/Utilities/StringHelper.hpp>

#include <SFML/Network/Dns.hpp>

#include <array>
#include <regex>

namespace Cx
{
    CommandLineContext::CommandLineContext(const int argc, char** argv)
    {
        for (std::size_t i = 1; i < argc; i++)
        {
            auto arg = std::string(argv[i]);
            if (arg == "-k" && i + 1 < argc)
            {
                m_libraries.push_back(std::string(argv[++i]));
                continue;
            }

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

    std::string CommandLineContext::GetFtpPath() const
    {
        if (GetArgumentCount() < 3)
            return std::string();

        return m_arguments[2];
    }

    std::vector<Gateway> CommandLineContext::GetGatewayInfo() const
    {
        if (GetArgumentCount() < 6)
            return {};

        try
        {
            const int gatewayCount = std::stoi(m_arguments[3]);

            // Different halls order for 3.10 compatibility
            auto gateways = std::vector<Gateway>();
            constexpr auto halls = std::array<MusicHall, 6>
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
                auto address = sf::IpAddress::Any;
                if (auto addresses = sf::Dns::resolve(m_arguments[4 + i * 2]); addresses.has_value())
                {
                    addresses->erase(
                        std::remove_if(addresses->begin(), addresses->end(), [](const auto& entry)
                        {
                            return !entry.isV4();
                        }),addresses->end()
                    );

                    if (!addresses->empty())
                        address = addresses->front();
                }

                gateways.push_back({
                    halls[i],
                    address,
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

    std::vector<std::string> CommandLineContext::GetLibraries() const
    {
        return m_libraries;
    }
}
