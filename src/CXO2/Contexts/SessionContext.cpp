#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/IO/Loaders/Chart/O2JamChartMetadataLoader.hpp>
#include <CXO2/IO/Loaders/Chart/O2JamMusicListLoader.hpp>

#include <CXO2/Network/Responses/CharacterInfoResponse.hpp>

#include <Genode/IO/LocalFileSystem.hpp>

#include <magic_enum/magic_enum.hpp>

#include <algorithm>
#include <charconv>
#include <filesystem>
#include <future>
#include <thread>
#include <unordered_map>

namespace Cx
{
    SessionContext::SessionContext(const std::string& token) :
        m_token(token),
        m_server(MusicHall::None),
        m_channelID(0)
    {
    }

    const std::string& SessionContext::GetToken() const
    {
        return m_token;
    }

    sf::String SessionContext::GetName() const
    {
        return m_name;
    }

    void SessionContext::SetName(const sf::String& name)
    {
        m_name = name;
    }

    Gender SessionContext::GetGender() const
    {
        return m_gender;
    }

    void SessionContext::SetGender(const Gender gender)
    {
        m_gender = gender;
    }

    Role SessionContext::GetRole() const
    {
        return m_role;
    }

    void SessionContext::SetRole(const Role role)
    {
        m_role = role;
    }

    std::int32_t SessionContext::GetLevel() const
    {
        return m_level;
    }

    void SessionContext::SetLevel(const std::int32_t level)
    {
        m_level = level;
    }

    std::int32_t SessionContext::GetExperience() const
    {
        return m_experience;
    }

    void SessionContext::SetExperience(const std::int32_t experience)
    {
        m_experience = experience;
    }

    SessionContext::RankStats SessionContext::GetRankStats() const
    {
        return m_rankStats;
    }

    void SessionContext::SetRankStats(const RankStats& rankStats)
    {
        m_rankStats = rankStats;
    }

    SessionContext::Wallet SessionContext::GetWallet() const
    {
        return m_wallet;
    }

    void SessionContext::SetWallet(const Wallet& wallet)
    {
        m_wallet = wallet;
    }

    const EquipmentSet& SessionContext::GetEquippedItemIDs() const
    {
        return m_equippedItemIDs;
    }

    const SessionContext::ItemList& SessionContext::GetInventory() const
    {
        return m_inventory;
    }

    const MusicList& SessionContext::GetMusicIDs() const
    {
        return m_musicIDs;
    }

    void SessionContext::Equip(const std::uint32_t itemID)
    {
        m_equippedItemIDs.insert(itemID);
    }

    void SessionContext::Unequip(const std::uint32_t itemID)
    {
        m_equippedItemIDs.erase(itemID);
    }

    void SessionContext::SetEquipment(const EquipmentSet& equippedItemIDs)
    {
        m_equippedItemIDs = equippedItemIDs;
    }

    void SessionContext::SetInventoryItem(const std::uint32_t slotID, const Item& item)
    {
        m_inventory[slotID] = item;
    }

    void SessionContext::AddInventoryItem(const std::uint32_t itemID)
    {
        m_inventory.push_back(itemID);
    }

    void SessionContext::UpdateFrom(const CharacterInfoResponse& response)
    {
        m_name       = response.Name;
        m_gender     = response.Gender;
        m_role       = response.Role;
        m_level      = response.Level;
        m_experience = response.Experience;

        m_rankStats.Rank  = 0;
        m_rankStats.Wins  = response.Wins;
        m_rankStats.Loses = response.Loses;
        m_rankStats.Draws = response.Draws;

        m_wallet.Gem  = response.Gem;
        m_wallet.Cash = response.Point;

        m_equippedItemIDs = response.EquippedItemIDs.GetContainer();

        m_inventory.clear();
        for (const std::uint32_t id : response.Inventory.GetContainer())
            m_inventory.push_back(id);

        m_musicIDs.clear();
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
        return m_server;
    }

    void SessionContext::SetMusicHall(const MusicHall hall)
    {
        m_server = hall;
    }

    unsigned int SessionContext::GetChannelID() const
    {
        return m_channelID;
    }

    void SessionContext::SetChannelID(const unsigned int channelId)
    {
        m_channelID = channelId;
    }

    void SessionContext::SetMusicListMode(const MusicListMode mode)
    {
        if (m_musicListMode == mode)
            return;

        m_musicListMode = mode;
        m_musicScanned  = false;
    }

    void SessionContext::SetAcquiredMusicIDs(std::unordered_set<std::uint32_t> musicIDs)
    {
        m_acquiredMusicIDs = std::move(musicIDs);
        m_musicScanned     = false;
    }

    const std::vector<ChartMetadata>& SessionContext::GetMusicList(const bool rescan) const
    {
        if (rescan || !m_musicScanned)
            ScanMusic();

        return m_displayMusicList;
    }

    const std::vector<ChartMetadata>& SessionContext::GetInstalledMusic(const bool rescan) const
    {
        if (rescan || !m_musicScanned)
            ScanMusic();

        return m_installedMusicList;
    }

    const std::vector<ChartMetadata>& SessionContext::GetNonPlayableMusicList() const
    {
        if (!m_musicScanned)
            ScanMusic();

        return m_nonPlayableMusicList;
    }

    void SessionContext::ScanMusic() const
    {
        const auto metaLoader = O2JamChartMetadataLoader();
        const auto musicListLoader = O2JamMusicListLoader();

        auto scanned  = std::vector<Gx::ResourcePtr<O2JamChartMetadata>>();
        auto failures = std::vector<ChartMetadata>();
        auto indices  = std::unordered_map<std::uint32_t, std::size_t>();

        struct ScanBatch
        {
            std::vector<Gx::ResourcePtr<O2JamChartMetadata>> Loaded;
            std::vector<ChartMetadata> Failures;
        };

        const auto files = Gx::FileSystem::Scan("o2ma*.ojn");
        const auto workerCount = std::clamp<std::size_t>(std::thread::hardware_concurrency(), 1, std::max<std::size_t>(files.size(), 1));
        const auto chunkSize   = (files.size() + workerCount - 1) / workerCount;

        auto workers = std::vector<std::future<ScanBatch>>();
        for (std::size_t w = 0; w < workerCount; w++)
        {
            workers.push_back(std::async(std::launch::async, [&files, &metaLoader, w, chunkSize]
            {
                auto batch = ScanBatch();
                for (std::size_t i = w * chunkSize; i < std::min(files.size(), (w + 1) * chunkSize); i++)
                {
                    if (auto meta = metaLoader.LoadFromFile(files[i]->GetName(), Gx::ResourceContext::Default))
                    {
                        batch.Loaded.push_back(std::move(meta));
                    }
                    else
                    {
                        auto failure   = ChartMetadata();
                        failure.Source = files[i]->GetName();
                        failure.Status = MusicStatus::InvalidFormat;

                        if (const auto stem = std::filesystem::path(files[i]->GetName()).stem().string(); stem.size() > 4)
                            std::from_chars(stem.data() + 4, stem.data() + stem.size(), failure.ID);

                        batch.Failures.push_back(std::move(failure));
                    }
                }

                return batch;
            }));
        }

        for (auto& worker : workers)
        {
            auto batch = worker.get();
            for (auto& meta : batch.Loaded)
            {
                indices[meta->ID] = scanned.size();
                scanned.push_back(std::move(meta));
            }

            for (auto& failure : batch.Failures)
                failures.push_back(std::move(failure));
        }

        if (m_musicListMode != MusicListMode::Free && !m_musicListLoaded)
        {
            m_musicListLoaded = true;
            if (Gx::FileSystem::Contains("OJNList.dat"))
                m_musicList = musicListLoader.LoadFromFile("OJNList.dat", Gx::ResourceContext::Default);
        }

        m_installedMusicList.clear();
        m_displayMusicList.clear();
        m_nonPlayableMusicList.clear();

        if (m_musicListMode == MusicListMode::Free || !m_musicList)
        {
            for (const auto& meta : scanned)
                m_installedMusicList.push_back(meta->ToChartMetadata());

            m_displayMusicList     = m_installedMusicList;
            m_nonPlayableMusicList = std::move(failures);
        }
        else
        {
            auto listed = std::unordered_set<std::uint32_t>();
            auto failureIndices = std::unordered_map<std::uint32_t, std::size_t>();
            for (std::size_t i = 0; i < failures.size(); i++)
                failureIndices[failures[i].ID] = i;

            for (const auto& chart : m_musicList->Charts)
            {
                listed.insert(chart.ID);

                auto merged = chart.ToChartMetadata();
                if (const auto it = indices.find(chart.ID); it != indices.end())
                {
                    const auto& local = *scanned[it->second];
                    if (chart.NoteCountEx == local.NoteCountEx && chart.NoteCountNx == local.NoteCountNx && chart.NoteCountHx == local.NoteCountHx)
                    {
                        merged.Source = local.Source;
                        merged.Status = MusicStatus::Playable;
                    }
                    else
                        merged.Status = MusicStatus::Corrupted;
                }
                else if (const auto failed = failureIndices.find(chart.ID); failed != failureIndices.end())
                {
                    merged.Source = failures[failed->second].Source;
                    merged.Status = MusicStatus::InvalidFormat;
                }
                else
                    merged.Status = MusicStatus::Missing;

                if (merged.Status == MusicStatus::Playable)
                    m_installedMusicList.push_back(merged);
                else
                    m_nonPlayableMusicList.push_back(merged);

                m_displayMusicList.push_back(std::move(merged));
            }

            if (m_musicListMode == MusicListMode::Mixed)
            {
                for (const auto& meta : scanned)
                {
                    if (listed.find(meta->ID) != listed.end())
                        continue;

                    auto entry = meta->ToChartMetadata();
                    m_installedMusicList.push_back(entry);
                    m_displayMusicList.push_back(std::move(entry));
                }

                for (auto& failure : failures)
                {
                    if (listed.find(failure.ID) == listed.end())
                        m_nonPlayableMusicList.push_back(std::move(failure));
                }
            }
        }

        if (m_acquiredMusicIDs.has_value())
        {
            const auto& acquired = m_acquiredMusicIDs.value();

            auto playable = std::vector<ChartMetadata>();
            for (auto& entry : m_installedMusicList)
            {
                if (acquired.find(entry.ID) == acquired.end())
                {
                    entry.Status = MusicStatus::Unacquired;
                    m_nonPlayableMusicList.push_back(std::move(entry));
                }
                else
                    playable.push_back(std::move(entry));
            }

            m_installedMusicList = std::move(playable);
            for (auto& entry : m_displayMusicList)
            {
                if (entry.Status == MusicStatus::Playable && acquired.find(entry.ID) == acquired.end())
                    entry.Status = MusicStatus::Unacquired;
            }
        }

        const auto comparer = [] (ChartMetadata& a, ChartMetadata& b)
        {
            return a.Levels[Difficulty::EX] < b.Levels[Difficulty::EX];
        };

        std::sort(m_installedMusicList.begin(), m_installedMusicList.end(), comparer);
        std::sort(m_displayMusicList.begin(), m_displayMusicList.end(), comparer);

        m_musicScanned = true;
    }

    void SessionContext::Load()
    {
        if (!Gx::LocalFileSystem::Instance().Contains("session.json"))
            return;

        const auto size = Gx::LocalFileSystem::Instance().GetFileSize("session.json");
        if (!size.has_value() || size.value() == 0)
            return;

        auto json = std::vector<std::byte>(size.value());
        if (const auto read = Gx::LocalFileSystem::Instance().ReadFile("session.json", json.data(), json.size()); !read.has_value() || read.value() != size.value())
            return;

        auto sessionDb = Gx::Json::parse(json, nullptr, false);
        if (sessionDb.is_discarded())
            return;

        if (const auto characterInfo = sessionDb.find("CharacterInfo"); characterInfo != sessionDb.end())
        {
            if (const auto it = characterInfo->find("name"); it != characterInfo->end() && it->is_string())
                m_name = StringTranscoder::Transcode(it->get<std::string>());

            if (const auto it = characterInfo->find("gender"); it != characterInfo->end() && it->is_string())
                m_gender = magic_enum::enum_cast<Gender>(it->get<std::string>(), magic_enum::case_insensitive).value_or(Gender::Male);

            if (const auto it = characterInfo->find("level"); it != characterInfo->end() && it->is_number_integer())
                m_level = it->get<int>();

            if (const auto it = characterInfo->find("gems"); it != characterInfo->end() && it->is_number_integer())
                m_wallet.Gem = it->get<unsigned int>();

            if (const auto it = characterInfo->find("cash"); it != characterInfo->end() && it->is_number_integer())
                m_wallet.Cash = it->get<unsigned int>();

            auto items = std::unordered_set<unsigned int>();
            if (const auto it = characterInfo->find("items"); it != characterInfo->end() && it->is_array())
            {
                m_inventory.clear();
                for (const auto id : it->items())
                {
                    if (id.value().is_number_unsigned() && id.value().is_number_integer())
                    {
                        const auto val = id.value().get<unsigned int>();
                        if (items.insert(val).second)
                            m_inventory.push_back(val);
                        else if (const auto in = std::find(m_inventory.begin(), m_inventory.end(), val); in != m_inventory.end())
                            in->Quantity++;
                    }
                }
            }

            if (const auto it = characterInfo->find("equipments"); it != characterInfo->end() && it->is_array())
            {
                m_equippedItemIDs.clear();
                auto set = std::unordered_set<unsigned int>();
                for (const auto id : it->items())
                {
                    if (id.value().is_number_unsigned() && id.value().is_number_integer())
                    {
                        const auto val = id.value().get<unsigned int>();
                        m_equippedItemIDs.insert(val);
                    }
                }
            }
        }
    }

    void SessionContext::Save() const
    {
        Gx::Json sessionDb;
        Gx::Json characterInfo;

        std::vector<std::uint32_t> items = {};
        for (auto& item : m_inventory)
        {
            for (std::size_t i = 0; i < item.Quantity; i++)
                items.push_back(item.ID);
        }

        characterInfo["name"]       = m_name.toAnsiString();
        characterInfo["gender"]     = magic_enum::enum_name(m_gender);
        characterInfo["level"]      = m_level;
        characterInfo["gems"]       = m_wallet.Gem;
        characterInfo["cash"]       = m_wallet.Cash;
        characterInfo["items"]      = items;
        characterInfo["equipments"] = m_equippedItemIDs;

        sessionDb["CharacterInfo"] = characterInfo;

        const auto json = sessionDb.dump(2);
        Gx::LocalFileSystem::Instance().WriteFile("session.json", json.c_str(), json.size());
    }
}
