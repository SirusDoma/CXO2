#include <CXO2/Contexts/RoomContext.hpp>
#include <CXO2/Contexts/GameContext.hpp>
#include <CXO2/Contexts/SessionContext.hpp>

#include <CXO2/Models/Map.hpp>

#include <CXO2/IO/Loaders/Chart/O2JamChartLoader.hpp>
#include <CXO2/IO/Loaders/Chart/O2JamChartMetadataLoader.hpp>

#include <CXO2/Constants/Identifiers/Cache.hpp>

#include <Genode/IO/FileSystem.hpp>

namespace Cx
{
    using namespace Constants::Identifiers;

    RoomContext::RoomContext(SessionContext& session, Gx::ResourceManager& resources) :
        m_session(session),
        m_resources(resources),
        m_sort(MusicSortMode::Level),
        m_order(MusicSortOrder::Ascending)
    {
    }

    RoomContext::~RoomContext()
    {
    }

    void RoomContext::Create(const std::uint32_t id)
    {
        m_state = Room{};
        m_state.ID = id;

        auto member = Member{};
        member.Name            = m_session.GetName();
        member.Gender          = m_session.GetGender();
        member.Level           = m_session.GetLevel();
        member.EquippedItemIDs = m_session.GetEquippedItemIDs();
        member.MusicIDs        = m_session.GetMusicIDs();

        Seat(0, member, Room::Team::A, true, true);
    }

    void RoomContext::Join(const std::uint32_t id)
    {
        m_state = Room{};
        m_state.ID = id;
    }

    void RoomContext::Leave()
    {
        m_state = Room{};
    }

    std::uint32_t RoomContext::GetID() const
    {
        return m_state.ID;
    }

    RoomState RoomContext::GetState() const
    {
        return m_state.State;
    }

    void RoomContext::SetState(const RoomState state)
    {
        m_state.State = state;
    }

    sf::String RoomContext::GetTitle() const
    {
        return m_state.Title;
    }

    void RoomContext::SetTitle(const sf::String& title)
    {
        m_state.Title = title;
    }

    bool RoomContext::IsLocked() const
    {
        return m_state.Locked;
    }

    void RoomContext::SetLocked(const bool locked)
    {
        m_state.Locked = locked;
    }

    ChartMetadata RoomContext::GetMusic() const
    {
        return m_state.Music;
    }

    void RoomContext::SetMusic(const ChartMetadata& metadata)
    {
        if (m_state.Music.ID == metadata.ID)
            return;

        m_state.Music = metadata;
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

    void RoomContext::SetMusicID(const std::uint32_t musicID)
    {
        const auto& musicList = m_session.GetMusicList();
        const auto it = std::find_if(musicList.begin(), musicList.end(), [musicID] (const auto& m)
        {
            return m.ID == musicID;
        });

        if (it != musicList.end())
            SetMusic(*it);
        else
            SetMusic(ChartMetadata{musicID});
    }

    Difficulty RoomContext::GetDifficulty() const
    {
        return m_state.Difficulty;
    }

    void RoomContext::SetDifficulty(const Difficulty difficulty)
    {
        m_state.Difficulty = difficulty;
    }

    GameMode RoomContext::GetMode() const
    {
        return m_state.Mode;
    }

    void RoomContext::SetMode(const GameMode mode)
    {
        m_state.Mode = mode;
    }

    float RoomContext::GetSpeed() const
    {
        return m_state.Speed;
    }

    void RoomContext::SetSpeed(const float speed)
    {
        m_state.Speed = speed;
    }

    SpeedMode RoomContext::GetSpeedMode() const
    {
        return m_state.SpeedMode;
    }

    void RoomContext::SetSpeedMode(const SpeedMode mode)
    {
        m_state.SpeedMode = mode;
    }

    Speed RoomContext::GetSpeedID() const
    {
        if (m_state.SpeedMode != SpeedMode::HiSpeed)
            return Speed::X05;

        return ToSpeed(m_state.Speed).value_or(Speed::X05);
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
        for (const auto& slot : m_state.Slots)
        {
            if (slot.State != Room::SlotState::Locked)
                capacity++;
        }

        return capacity;
    }

    LevelCategory RoomContext::GetRandomLevel() const
    {
        return m_state.Random;
    }

    void RoomContext::SetRandomLevel(const LevelCategory random)
    {
        m_state.Random = random;
    }

    bool RoomContext::IsRandomActive() const
    {
        return m_state.Random != static_cast<LevelCategory>(0);
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

    Map RoomContext::GetMap() const
    {
        return m_state.Map;
    }

    void RoomContext::SetMap(const Map map)
    {
        m_state.Map = map;
    }

    std::uint8_t RoomContext::GetEffectID() const
    {
        return m_state.EffectID;
    }

    void RoomContext::SetEffectID(std::uint8_t effectID)
    {
        m_state.EffectID = effectID;
    }

    std::uint8_t RoomContext::GetMinLevelLimit() const
    {
        return m_state.MinLevelLimit;
    }

    std::uint8_t RoomContext::GetMaxLevelLimit() const
    {
        return m_state.MaxLevelLimit;
    }

    void RoomContext::SetLevelLimits(const std::uint8_t minLevelLimit, const std::uint8_t maxLevelLimit)
    {
        m_state.MinLevelLimit = minLevelLimit;
        m_state.MaxLevelLimit = maxLevelLimit;
    }

    void RoomContext::Seat(const std::size_t index, const Member& member, const Room::Team team, const bool ready, const bool isMaster)
    {
        auto& slot           = m_state.Slots[index];
        slot.Name            = member.Name;
        slot.Gender          = member.Gender;
        slot.Level           = member.Level;
        slot.EquippedItemIDs = member.EquippedItemIDs;
        slot.MusicIDs        = member.MusicIDs;
        slot.State           = Room::SlotState::Occupied;
        slot.IsMaster        = isMaster;
        slot.Ready           = isMaster || ready;
        slot.Team            = team;
    }

    void RoomContext::Vacate(const std::size_t index)
    {
        auto& slot = m_state.Slots[index];
        slot = Room::Slot{};
    }

    void RoomContext::Lock(const std::size_t index)
    {
        auto& slot = m_state.Slots[index];
        if (slot.State == Room::SlotState::Occupied)
            throw Gx::InvalidOperationException();

        slot = Room::Slot{};
        slot.State = Room::SlotState::Locked;
    }

    void RoomContext::Unlock(const std::size_t index)
    {
        auto& slot = m_state.Slots[index];
        if (slot.State == Room::SlotState::Occupied)
            throw Gx::InvalidOperationException();

        slot = Room::Slot{};
    }

    void RoomContext::PromoteMaster(const std::size_t index)
    {
        if (index >= m_state.Slots.size())
            throw Gx::ArgumentOutOfRangeException();

        for (std::size_t i = 0; i < m_state.Slots.size(); i++)
        {
            auto& slot    = m_state.Slots[i];
            slot.IsMaster = index == i;
            slot.Ready    = slot.IsMaster || slot.Ready;
        }
    }

    void RoomContext::SetMemberReady(const std::size_t index, const bool ready)
    {
        if (index >= m_state.Slots.size())
            throw Gx::ArgumentOutOfRangeException();

        m_state.Slots[index].Ready = ready;
    }

    void RoomContext::SetMemberTeam(const std::size_t index, const Room::Team team)
    {
        if (index >= m_state.Slots.size())
            throw Gx::ArgumentOutOfRangeException();

        m_state.Slots[index].Team = team;
    }

    void RoomContext::SetMemberTeamColor(const std::size_t index, const sf::Color& color)
    {
        if (index >= m_state.Slots.size())
            throw Gx::ArgumentOutOfRangeException();

        m_state.Slots[index].TeamColor = color;
    }

    void RoomContext::SetMemberLevel(const std::size_t index, const std::int32_t level)
    {
        if (index >= m_state.Slots.size())
            throw Gx::ArgumentOutOfRangeException();

        m_state.Slots[index].Level = level;
    }

    void RoomContext::SetMemberEquipment(const std::size_t index, const EquipmentSet& equippedItemIDs)
    {
        if (index >= m_state.Slots.size())
            throw Gx::ArgumentOutOfRangeException();

        m_state.Slots[index].EquippedItemIDs = equippedItemIDs;
    }

    const Room::Slot& RoomContext::GetMaster() const
    {
        const auto it = std::find_if(m_state.Slots.begin(), m_state.Slots.end(), [] (const auto& slot)
        {
            return slot.IsMaster;
        });

        return *it;
    }

    const Room::Slot& RoomContext::GetCurrentSlot() const
    {
        return m_state.Slots[GetCurrentSlotIndex()];
    }

    const Room::Slot& RoomContext::GetSlot(const std::size_t index) const
    {
        return m_state.Slots[index];
    }

    std::size_t RoomContext::GetCurrentSlotIndex() const
    {
        const auto it = std::find_if(m_state.Slots.begin(), m_state.Slots.end(), [this] (const auto& slot)
        {
            return slot.State == Room::SlotState::Occupied && slot.Name == m_session.GetName();
        });

        return static_cast<std::size_t>(std::distance(m_state.Slots.begin(), it));
    }

    std::unordered_set<std::uint32_t> RoomContext::GetAvailableMusicIDs() const
    {
        auto result = std::unordered_set<std::uint32_t>{};
        const auto& musicList = m_session.GetInstalledMusic();

        for (const auto& music : musicList)
        {
            bool available = true;
            for (const auto& slot : m_state.Slots)
            {
                if (slot.State != Room::SlotState::Occupied || slot.Name == m_session.GetName())
                    continue;

                available = slot.MusicIDs.find(music.ID) != slot.MusicIDs.end();
                if (!available)
                    break;
            }

            if (available)
                result.insert(music.ID);
        }

        return result;
    }

    GameContext RoomContext::CreateGameContext() const
    {
        auto chart = std::make_unique<Chart>();
        chart->Source = m_state.Music.Source;
        chart->SetMetadata(m_state.Music);

        auto game = GameContext{m_session};
        game.SetChart(std::move(chart));

        game.SetDifficulty(m_state.Difficulty);
        game.SetMode(m_state.Mode);
        game.SetSpeed(m_state.Speed);
        game.SetSpeedMode(m_state.SpeedMode);
        game.SetMapID(m_state.Map.ID);
        game.SetEffectID(m_state.EffectID);

        return game;
    }
}
