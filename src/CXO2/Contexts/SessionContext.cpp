#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/IO/Loaders/Chart/O2JamChartMetadataLoader.hpp>

#include <Genode/IO/LocalFileSystem.hpp>

#include <magic_enum/magic_enum.hpp>

namespace Cx
{
    SessionContext::SessionContext(const std::string& token) :
        m_token(token),
        m_characterInfo(),
        m_server(MusicHall::None),
        m_channelID(0)
    {
    }

    const std::string& SessionContext::GetToken() const
    {
        return m_token;
    }

    CharacterInfo& SessionContext::GetCharacterInfo()
    {
        return m_characterInfo;
    }

    void SessionContext::SetCharacterInfo(const CharacterInfo& CharacterInfo)
    {
        m_characterInfo = CharacterInfo;
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

    const std::vector<ChartMetadata>& SessionContext::GetInstalledMusic(const bool rescan) const
    {
        if (rescan || m_installedMusicList.empty())
        {
            m_installedMusicList.clear();
            const auto metaLoader = O2JamChartMetadataLoader();

            for (const auto& file : Gx::FileSystem::Scan("o2ma*.ojn"))
            {
                if (const auto meta = metaLoader.LoadFromFile(file->GetName(), Gx::ResourceContext::Default))
                    m_installedMusicList.push_back(meta->ToChartMetadata());
            }

            std::sort(m_installedMusicList.begin(), m_installedMusicList.end(), [this] (ChartMetadata& a, ChartMetadata& b)
            {
                return a.Levels[Difficulty::EX] < b.Levels[Difficulty::EX];
            });
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

        auto json = std::vector<std::byte>(size.value());
        if (const auto read = Gx::LocalFileSystem::Instance().ReadFile("session.json", json.data(), json.size()); !read.has_value() || read.value() != size.value())
            return;

        auto sessionDb = Gx::Json::parse(json, nullptr, false);
        if (sessionDb.is_discarded())
            return;

        if (const auto CharacterInfoInfo = sessionDb.find("CharacterInfo"); CharacterInfoInfo != sessionDb.end())
        {
            if (const auto it = CharacterInfoInfo->find("name"); it != CharacterInfoInfo->end() && it->is_string())
                m_characterInfo.Name = StringTranscoder::Transcode(it->get<std::string>());

            if (const auto it = CharacterInfoInfo->find("gender"); it != CharacterInfoInfo->end() && it->is_string())
                m_characterInfo.Gender = magic_enum::enum_cast<Gender>(it->get<std::string>(), magic_enum::case_insensitive).value_or(Gender::Male);

            if (const auto it = CharacterInfoInfo->find("level"); it != CharacterInfoInfo->end() && it->is_number_integer())
                m_characterInfo.Level = it->get<int>();

            if (const auto it = CharacterInfoInfo->find("gems"); it != CharacterInfoInfo->end() && it->is_number_integer())
                m_characterInfo.Wallet.Gem = it->get<unsigned int>();

            if (const auto it = CharacterInfoInfo->find("cash"); it != CharacterInfoInfo->end() && it->is_number_integer())
                m_characterInfo.Wallet.Cash = it->get<unsigned int>();

            auto items = std::unordered_set<unsigned int>();
            if (const auto it = CharacterInfoInfo->find("items"); it != CharacterInfoInfo->end() && it->is_array())
            {
                m_characterInfo.Inventory.clear();
                for (const auto id : it->items())
                {
                    if (id.value().is_number_unsigned() && id.value().is_number_integer())
                    {
                        const auto val = id.value().get<unsigned int>();
                        if (items.insert(val).second)
                            m_characterInfo.Inventory.push_back(val);
                        else if (const auto in = std::find(m_characterInfo.Inventory.begin(), m_characterInfo.Inventory.end(), val); in != m_characterInfo.Inventory.end())
                            in->Quantity++;
                    }
                }
            }

            if (const auto it = CharacterInfoInfo->find("equipments"); it != CharacterInfoInfo->end() && it->is_array())
            {
                m_characterInfo.EquippedItemIDs.clear();
                auto set = std::unordered_set<unsigned int>();
                for (const auto id : it->items())
                {
                    if (id.value().is_number_unsigned() && id.value().is_number_integer())
                    {
                        const auto val = id.value().get<unsigned int>();
                        m_characterInfo.EquippedItemIDs.insert(val);
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
        for (auto& item : m_characterInfo.Inventory)
        {
            for (std::size_t i = 0; i < item.Quantity; i++)
                items.push_back(item.ID);
        }

        characterInfo["name"]       = m_characterInfo.Name.toAnsiString();
        characterInfo["gender"]     = magic_enum::enum_name(m_characterInfo.Gender);
        characterInfo["level"]      = m_characterInfo.Level;
        characterInfo["gems"]       = m_characterInfo.Wallet.Gem;
        characterInfo["cash"]       = m_characterInfo.Wallet.Cash;
        characterInfo["items"]      = items;
        characterInfo["equipments"] = m_characterInfo.EquippedItemIDs;

        sessionDb["CharacterInfo"] = characterInfo;

        const auto json = sessionDb.dump(2);
        Gx::LocalFileSystem::Instance().WriteFile("session.json", json.c_str(), json.size());
    }
}
