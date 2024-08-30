#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/IO/Loaders/Chart/ChartMetadataLoader.hpp>

SessionContext::SessionContext(const Player &player) :
    m_player(player),
    m_hall(MusicHall::None),
    m_channelID(0),
    m_room(),
    m_lastResult()
{
}

Player& SessionContext::GetCurrentPlayer()
{
    return m_player;
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

const std::array<ScoreResultItem, 8> & SessionContext::GetLatestScoreResults() const
{
    return m_lastResult;
}

void SessionContext::SetLatestScoreResults(const std::array<ScoreResultItem, 8> &result)
{
    for (int i = 0; i < m_lastResult.size(); i++)
        m_lastResult[i] = result[i];

    std::sort(m_lastResult.begin(), m_lastResult.end(), [] (auto& a, auto& b) { return a.ScorePoint > b.ScorePoint; });
}

const std::vector<ChartMetadata> &SessionContext::GetInstalledMusic(const bool rescan) const
{
    if (rescan || m_installedMusicList.empty())
    {
        m_installedMusicList.clear();
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
