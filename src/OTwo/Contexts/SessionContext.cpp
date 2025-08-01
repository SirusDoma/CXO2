#include <magic_enum/magic_enum.hpp>
#include <Genode/IO/FileSystem/LocalFileSystem.hpp>
#include <OTwo/Contexts/SessionContext.hpp>
#include <OTwo/IO/Loaders/Chart/ChartMetadataLoader.hpp>

SessionContext::SessionContext(const std::string& token, const Player& player) :
    m_token(token),
    m_player(player),
    m_hall(MusicHall::None),
    m_channelID(0),
    m_room(),
    m_lastResult()
{
}

const std::string& SessionContext::GetToken() const
{
    return m_token;
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

const Room& SessionContext::GetCurrentRoom() const
{
    return m_room;
}

void SessionContext::SetCurrentRoom(const Room& room)
{
    m_room = room;
}

const std::array<ScoreResultItem, 8> & SessionContext::GetLatestScoreResults() const
{
    return m_lastResult;
}

void SessionContext::SetLatestScoreResults(const std::array<ScoreResultItem, 8> &result)
{
    for (std::size_t i = 0; i < m_lastResult.size(); i++)
        m_lastResult[i] = result[i];

    std::sort(m_lastResult.begin(), m_lastResult.end(), [] (auto& a, auto& b) { return a.ScorePoint > b.ScorePoint; });
}

const std::vector<ChartMetadata> &SessionContext::GetInstalledMusic(const bool rescan) const
{
    if (rescan || m_installedMusicList.empty())
    {
        m_installedMusicList.clear();
        const auto metaLoader = ChartMetadataLoader();
        for (const auto& file : Gx::FileSystem::Scan("o2ma*.ojn"))
        {
            auto name = file->GetName();
            auto meta = metaLoader.LoadFromFile(file->GetName(), Gx::ResourceContext::Default);
            m_installedMusicList.push_back(*meta);
        }
    }

    return m_installedMusicList;
}

void SessionContext::Load()
{
    if (!Gx::LocalFileSystem::Instance().Contains("session.json"))
        return;

    const auto size = Gx::LocalFileSystem::Instance().GetFileSize("session.json");
    if (!size.has_value() || size.value() == 0)
        return;

    auto json = std::vector<std::uint8_t>(size.value());
    if (const auto read = Gx::LocalFileSystem::Instance().ReadFile("session.json", json.data(), json.size()); !read.has_value() || read.value() != size.value())
        return;

    auto sessionDb = Gx::Json::parse(json, nullptr, false);
    if (sessionDb.is_discarded())
        return;

    if (const auto playerInfo = sessionDb.find("player"); playerInfo != sessionDb.end())
    {
        if (const auto it = playerInfo->find("name"); it != playerInfo->end() && it->is_string())
            m_player.Name = it->get<std::string>();

        if (const auto it = playerInfo->find("gender"); it != playerInfo->end() && it->is_string())
            m_player.Gender = magic_enum::enum_cast<Gender>(it->get<std::string>(), magic_enum::case_insensitive).value_or(Gender::Male);

        if (const auto it = playerInfo->find("level"); it != playerInfo->end() && it->is_number_integer())
            m_player.Level = it->get<int>();

        if (const auto it = playerInfo->find("gems"); it != playerInfo->end() && it->is_number_integer())
            m_player.Gem = it->get<unsigned int>();

        if (const auto it = playerInfo->find("cash"); it != playerInfo->end() && it->is_number_integer())
            m_player.Cash = it->get<unsigned int>();

        auto items = std::unordered_set<unsigned int>();
        if (const auto it = playerInfo->find("items"); it != playerInfo->end() && it->is_array())
        {
            m_player.Inventory.clear();
            for (const auto id : it->items())
            {
                if (id.value().is_number_unsigned() && id.value().is_number_integer())
                {
                    const auto val = id.value().get<unsigned int>();
                    if (items.insert(val).second)
                        m_player.Inventory.push_back(val);
                    else
                        m_player.ItemQuantities[val]++;
                }
            }
        }

        if (const auto it = playerInfo->find("equipments"); it != playerInfo->end() && it->is_array())
        {
            m_player.EquippedItemIDs.clear();
            auto set = std::unordered_set<unsigned int>();
            for (const auto id : it->items())
            {
                if (id.value().is_number_unsigned() && id.value().is_number_integer())
                {
                    const auto val = id.value().get<unsigned int>();
                    if (items.find(val) != items.end() && set.insert(val).second)
                        m_player.EquippedItemIDs.push_back(val);
                }
            }
        }
    }
}

void SessionContext::Save() const
{
    Gx::Json sessionDb;
    Gx::Json playerInfo;

    playerInfo["name"]       = m_player.Name;
    playerInfo["gender"]     = magic_enum::enum_name(m_player.Gender);
    playerInfo["level"]      = m_player.Level;
    playerInfo["gems"]       = m_player.Gem;
    playerInfo["cash"]       = m_player.Cash;
    playerInfo["items"]      = m_player.Inventory;
    playerInfo["equipments"] = m_player.EquippedItemIDs;

    sessionDb["player"] = playerInfo;

    const auto json = sessionDb.dump(2);
    Gx::LocalFileSystem::Instance().WriteFile("session.json", json.c_str(), json.size());
}
