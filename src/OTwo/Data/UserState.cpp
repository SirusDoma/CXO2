#include <OTwo/Data/UserState.hpp>
#include <OTwo/IO/Loaders/Chart/ChartMetadataLoader.hpp>

const PlayerData &UserState::GetCurrentPlayer() const
{
    return m_player;
}

void UserState::SetCurrentPlayer(const PlayerData &player)
{
    m_player = player;
}

Planet UserState::GetPlanet() const
{
    return m_planet;
}

void UserState::SetPlanet(const Planet planet)
{
    m_planet = planet;
}

MusicHall UserState::GetMusicHall() const
{
    return m_hall;
}

void UserState::SetMusicHall(const MusicHall hall)
{
    m_hall = hall;
}

unsigned int UserState::GetChannelID() const
{
    return m_channelID;
}

void UserState::SetChannelID(const unsigned int channelId)
{
    m_channelID = channelId;
}

const RoomData &UserState::GetCurrentRoom() const
{
    return m_room;
}

void UserState::SetCurrentRoom(const RoomData &room)
{
    m_room = room;
}

const std::vector<ChartMetadata> &UserState::GetInstalledMusic() const
{
    if (m_installedMusicList.empty())
    {
        auto metaLoader = ChartMetadataLoader();
        for (const auto &file : Gx::FileSystem::Scan("o2ma*.ojn"))
        {
            auto name = file.GetName();
            auto meta = metaLoader.LoadFromFile(file.GetName(), Gx::ResourceContext::Default);
            m_installedMusicList.push_back(*meta);
        }
    }

    return m_installedMusicList;
}
