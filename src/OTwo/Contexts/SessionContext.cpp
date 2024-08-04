#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/IO/Loaders/Chart/ChartMetadataLoader.hpp>

const Player &SessionContext::GetCurrentPlayer() const
{
    return m_player;
}

void SessionContext::SetCurrentPlayer(const Player &player)
{
    m_player = player;
}

Planet SessionContext::GetPlanet() const
{
    return m_planet;
}

void SessionContext::SetPlanet(const Planet planet)
{
    m_planet = planet;
}

MusicHall SessionContext::GetMusicHall() const
{
    return m_hall;
}

void SessionContext::SetMusicHall(const MusicHall hall)
{
    m_hall = hall;
}

unsigned int SessionContext::GetChannelID() const
{
    return m_channelID;
}

void SessionContext::SetChannelID(const unsigned int channelId)
{
    m_channelID = channelId;
}

const Room &SessionContext::GetCurrentRoom() const
{
    return m_room;
}

void SessionContext::SetCurrentRoom(const Room &room)
{
    m_room = room;
}

const std::vector<ChartMetadata> &SessionContext::GetInstalledMusic(const bool rescan) const
{
    if (rescan || m_installedMusicList.empty())
    {
        const auto metaLoader = ChartMetadataLoader();
        for (const auto &file : Gx::FileSystem::Scan("o2ma*.ojn"))
        {
            auto name = file->GetName();
            auto meta = metaLoader.LoadFromFile(file->GetName(), Gx::ResourceContext::Default);
            m_installedMusicList.push_back(*meta);
        }
    }

    return m_installedMusicList;
}
