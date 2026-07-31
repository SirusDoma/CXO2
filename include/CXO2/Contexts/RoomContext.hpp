#pragma once

#include <CXO2/Metadata/Chart/O2JamChartMetadata.hpp>
#include <CXO2/Metadata/Chart/ChartMetadata.hpp>
#include <CXO2/Models/Character.hpp>
#include <CXO2/Models/Map.hpp>
#include <CXO2/Models/Room.hpp>

#include <SFML/Graphics/Color.hpp>

#include <array>


namespace Gx
{
    class ResourceManager;
}

namespace Cx
{
    enum class StartGameResult : std::uint32_t;

    enum class MusicSortMode
    {
        ID,
        Title,
        Level,
        Duration
    };

    enum class MusicSortOrder
    {
        None,
        Ascending,
        Descending
    };


    struct GameContext;

    class SessionContext;
    class RoomContext
    {
    public:
        static constexpr std::uint8_t MaxCapacity = Room::MaxCapacity;

        struct Member
        {
            sf::String   Name;
            Cx::Gender   Gender{};
            std::int32_t Level{};
            EquipmentSet EquippedItemIDs;
            MusicList    MusicIDs;
        };

        explicit RoomContext(SessionContext& session, Gx::ResourceManager& resources);
        ~RoomContext();

        void Create(std::uint32_t id);
        void Join(std::uint32_t id);
        void Leave();

        std::uint32_t GetID() const;

        RoomState GetState() const;
        void SetState(RoomState state);

        sf::String GetTitle() const;
        void SetTitle(const sf::String& title);

        bool IsLocked() const;
        void SetLocked(bool locked);

        ChartMetadata GetMusic() const;
        void SetMusic(const ChartMetadata& metadata);
        void SetMusicID(std::uint32_t musicID);

        Difficulty GetDifficulty() const;
        void SetDifficulty(Difficulty difficulty);

        GameMode GetMode() const;
        void SetMode(GameMode mode);

        float GetSpeed() const;
        void SetSpeed(float speed);

        SpeedMode GetSpeedMode() const;
        void SetSpeedMode(SpeedMode mode);

        Speed GetSpeedID() const;
        void SetSpeedID(Speed speedID);

        std::uint8_t GetCapacity() const;

        LevelCategory GetRandomLevel() const;
        void SetRandomLevel(LevelCategory random);
        bool IsRandomActive() const;

        MusicSortMode GetMusicSortMode() const;
        void SetMusicSortMode(MusicSortMode sort);

        MusicSortOrder GetMusicSortOrder() const;
        void SetMusicSortOrder(MusicSortOrder order);

        Map GetMap() const;
        void SetMap(Map map);

        std::uint8_t GetEffectID() const;
        void SetEffectID(std::uint8_t effectID);

        std::uint8_t GetMinLevelLimit() const;
        std::uint8_t GetMaxLevelLimit() const;
        void SetLevelLimits(std::uint8_t minLevelLimit, std::uint8_t maxLevelLimit);

        void Seat(std::size_t index, const Member& member, Room::Team team, bool ready, bool isMaster = false);
        void Vacate(std::size_t index);
        void Lock(std::size_t index);
        void Unlock(std::size_t index);
        void PromoteMaster(std::size_t index);
        void SetMemberReady(std::size_t index, bool ready);
        void SetMemberTeam(std::size_t index, Room::Team team);
        void SetMemberTeamColor(std::size_t index, const sf::Color& color);
        void SetMemberLevel(std::size_t index, std::int32_t level);
        void SetMemberEquipment(std::size_t index, const EquipmentSet& equippedItemIDs);

        const Room::Slot& GetMaster() const;
        const Room::Slot& GetCurrentSlot() const;
        const Room::Slot& GetSlot(std::size_t index) const;
        std::size_t GetCurrentSlotIndex() const;

        std::unordered_set<std::uint32_t> GetAvailableMusicIDs() const;

        GameContext CreateGameContext() const;

    private:
        SessionContext& m_session;
        Gx::ResourceManager& m_resources;

        Room m_state;
        MusicSortMode  m_sort;
        MusicSortOrder m_order;
    };
}
