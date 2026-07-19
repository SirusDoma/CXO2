#include <CXO2/Contexts/RoomContext.hpp>
#include <CXO2/Contexts/SessionContext.hpp>

#include <CXO2/Services/WaitingService.hpp>
#include <CXO2/Network/RoomInfo.hpp>

#include <CXO2/IO/Loaders/Chart/O2JamChartLoader.hpp>
#include <CXO2/IO/Loaders/Chart/O2JamChartMetadataLoader.hpp>

#include <CXO2/StringTable/Identifiers/Cache.hpp>

#include <Genode/IO/FileSystem.hpp>

namespace Cx
{
    using namespace StringTable::Identifiers;

    RoomContext::RoomContext(SessionContext& session, WaitingService& service, Gx::ResourceManager& resources) :
        m_session(session),
        m_service(service),
        m_resources(resources),
        m_difficulty(Difficulty::EX),
        m_speed(1.0f),
        m_random(static_cast<LevelCategory>(0)),
        m_sort(MusicSortMode::Level),
        m_order(MusicSortOrder::Ascending)
    {
    }

    RoomContext::~RoomContext()
    {
    }

    std::uint32_t RoomContext::GetID() const
    {
        return m_id;
    }

    void RoomContext::SetID(const std::uint32_t id)
    {
        m_id = id;
    }

    RoomState RoomContext::GetState() const
    {
        return m_state;
    }

    void RoomContext::SetState(const RoomState state)
    {
        m_state = state;
    }

    sf::String RoomContext::GetTitle() const
    {
        return m_title;
    }

    void RoomContext::SetTitle(const sf::String& title)
    {
        m_title = title;
    }

    bool RoomContext::IsLocked() const
    {
        return m_locked;
    }

    void RoomContext::SetLocked(const bool locked)
    {
        m_locked = locked;
    }

    ChartMetadata RoomContext::GetMusic() const
    {
        return m_music;
    }

    void RoomContext::SetMusic(const ChartMetadata& metadata)
    {
        if (m_music.ID == metadata.ID)
            return;

        m_music = metadata;
        if (!metadata.Source.empty() && Gx::FileSystem::Contains(metadata.Source))
        {
            if (const auto chartMetadata = O2JamChartMetadataLoader().LoadFromFile(metadata.Source, Gx::ResourceContext::Default))
            {
                if (auto image = O2JamChartLoader::LoadCoverArt(*chartMetadata, Gx::ResourceContext::Default); image)
                    m_resources.Store<sf::Image>(Resource::Cache::IDC_IMAGE_STATE_LOADING_COVER, std::move(image), Gx::CacheMode::Update);
                else
                    m_resources.Destroy<sf::Image>(Resource::Cache::IDC_IMAGE_STATE_LOADING_COVER);
            }
        }
        else
            m_resources.Destroy<sf::Image>(Resource::Cache::IDC_IMAGE_STATE_LOADING_COVER);
    }

    Difficulty RoomContext::GetDifficulty() const
    {
        return m_difficulty;
    }

    void RoomContext::SetDifficulty(const Difficulty difficulty)
    {
        m_difficulty = difficulty;
    }

    GameMode RoomContext::GetMode() const
    {
        return m_mode;
    }

    void RoomContext::SetMode(const GameMode mode)
    {
        m_mode = mode;
    }

    float RoomContext::GetSpeed() const
    {
        return m_speed;
    }

    void RoomContext::SetSpeed(const float speed)
    {
        m_speed = speed;
    }

    SpeedMode RoomContext::GetSpeedMode() const
    {
        return m_speedMode;
    }

    void RoomContext::SetSpeedMode(const SpeedMode mode)
    {
        m_speedMode = mode;
    }

    Speed RoomContext::GetSpeedID() const
    {
        if (m_speedMode != SpeedMode::HiSpeed)
            return Speed::X05;

        return ToSpeed(m_speed).value_or(Speed::X05);
    }

    void RoomContext::SetSpeedID(const Speed speedID)
    {
        SetSpeed(ToSpeedValue(speedID).value_or(0.5f));
        // The speed mode is inferred from the SpeedButton, not from the network
        // SetSpeedMode(ToSpeedMode(speedID));
    }

    std::uint8_t RoomContext::GetCapacity() const
    {
        std::uint8_t capacity = 0;
        for (const auto& slot : m_slots)
        {
            if (slot.State != RoomSlotState::Locked)
                capacity++;
        }

        return capacity;
    }

    std::uint8_t RoomContext::GetMinLevelLimit() const
    {
        return m_minLevelLimit;
    }

    void RoomContext::SetMinLevelLimit(const std::uint8_t minLevelLimit)
    {
        m_minLevelLimit = minLevelLimit;
    }

    std::uint8_t RoomContext::GetMaxLevelLimit() const
    {
        return m_maxLevelLimit;
    }

    void RoomContext::SetMaxLevelLimit(const std::uint8_t maxLevelLimit)
    {
        m_maxLevelLimit = maxLevelLimit;
    }

    LevelCategory RoomContext::GetRandomLevel() const
    {
        return m_random;
    }

    void RoomContext::SetRandomLevel(const LevelCategory random)
    {
        m_random = random;
    }

    bool RoomContext::IsRandomActive() const
    {
        return m_random != static_cast<LevelCategory>(0);
    }

    MusicSortMode RoomContext::GetMusicSortMode() const
    {
        return m_sort;
    }

    void RoomContext::SetMusicSortMode(const MusicSortMode sort)
    {
        m_sort = sort;
    }

    MusicSortOrder RoomContext::GetMusicSortOrder() const
    {
        return m_order;
    }

    void RoomContext::SetMusicSortOrder(const MusicSortOrder order)
    {
        m_order = order;
    }

    std::uint8_t RoomContext::GetMapID() const
    {
        return m_mapID;
    }

    void RoomContext::SetMapID(const std::uint8_t mapID)
    {
        m_mapID = mapID;
    }

    std::uint8_t RoomContext::GetRandomizedMapID() const
    {
        return m_randomizedMapID;
    }

    void RoomContext::SetRandomizedMapID(const std::uint8_t randomizedMapID)
    {
        m_randomizedMapID = randomizedMapID;
    }

    std::uint8_t RoomContext::GetEffectID() const
    {
        return m_effectID;
    }

    void RoomContext::SetEffectID(std::uint8_t effectID)
    {
        m_effectID = effectID;
    }

    RoomSlot& RoomContext::GetMaster()
    {
        const auto it = std::find_if(m_slots.begin(), m_slots.end(), [] (const auto& slot)
        {
            return slot.IsMaster;
        });

        return *it;
    }

    RoomSlot& RoomContext::GetCurrentSlot()
    {
        const auto it = std::find_if(m_slots.begin(), m_slots.end(), [this] (const auto& slot)
        {
            return slot.Member.has_value() && slot.Member->Name == m_session.GetCharacterInfo().Name;
        });

        return *it;
    }


    RoomSlot& RoomContext::GetSlot(const std::size_t index)
    {
        return m_slots[index];
    }

    std::unordered_set<std::uint32_t> RoomContext::GetAvailableMusicIDs() const
    {
        auto result = std::unordered_set<std::uint32_t>{};
        const auto& musicList = m_session.GetInstalledMusic();

        for (const auto& music : musicList)
        {
            bool available = true;
            for (const auto& slot : m_slots)
            {
                if (slot.State != RoomSlotState::Occupied || !slot.Member.has_value() || slot.Member->Name == m_session.GetCharacterInfo().Name)
                    continue;

                available = slot.Member->MusicIDs.find(music.ID) != slot.Member->MusicIDs.end();
                if (!available)
                    break;
            }

            if (available)
                result.insert(music.ID);
        }

        return result;
    }

    void RoomContext::UpdateFrom(const RoomInfo& room)
    {
        SetID(room.ID);
        SetState(room.State);
        SetTitle(room.Title);
        SetLocked(room.Locked);
        SetDifficulty(room.Difficulty);
        SetMode(room.Mode);
        SetSpeedID(room.Speed);
        SetSpeedMode(ToSpeedMode(room.Speed));
        SetMinLevelLimit(room.MinLevelLimit);
        SetMaxLevelLimit(room.MaxLevelLimit);

        m_slots = {};
        if (room.MusicID > 0)
        {
            SetRandomLevel(static_cast<LevelCategory>(0));

            const auto& musicList = m_session.GetInstalledMusic();
            const auto it = std::find_if(musicList.begin(), musicList.end(), [id = room.MusicID] (const auto& m)
            {
                return m.ID == id;
            });

            if (it != musicList.end())
                SetMusic(*it);
            else
                SetMusic(ChartMetadata{});
        }
        else
        {
            SetRandomLevel(LevelCategory::Level1);
        }
    }
}
