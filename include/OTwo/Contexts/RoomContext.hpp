#pragma once

#include <OTwo/Metadata/Chart/O2JamChartMetadata.hpp>
#include <OTwo/Metadata/Chart/ChartMetadata.hpp>
#include <OTwo/Models/Character.hpp>

#include <SFML/Graphics/Color.hpp>

#include <mutex>
#include <optional>

enum class RoomSlotEventType : std::uint8_t;
enum class StartGameResult : std::uint32_t;

namespace Gx
{
    class ResourceManager;
}

enum class RoomTeam : std::uint8_t;
enum class RoomState : std::uint8_t;
enum class RoomSlotState : std::uint32_t;

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

struct RoomInfo;
struct RoomSlot
{
    std::optional<CharacterInfo> Member{std::nullopt};
    RoomSlotState State{};
    bool IsMaster{};
    bool Ready{};
    RoomTeam Team{};
    sf::Color TeamColor{sf::Color::Transparent};
};


class SessionContext;
class WaitingService;
class RoomContext
{
public:
    static constexpr std::uint8_t MaxCapacity = 8;

    explicit RoomContext(SessionContext& session, WaitingService& service, Gx::ResourceManager& resources);
    ~RoomContext();

    std::uint32_t GetID() const;
    void SetID(std::uint32_t id);

    RoomState GetState() const;
    void SetState(RoomState state);

    sf::String GetTitle() const;
    void SetTitle(const sf::String& title);

    bool IsLocked() const;
    void SetLocked(bool locked);

    ChartMetadata GetMusic() const;
    void SetMusic(const ChartMetadata& metadata);

    Difficulty GetDifficulty() const;
    void SetDifficulty(Difficulty difficulty);

    GameMode GetMode() const;
    void SetMode(GameMode mode);

    float GetSpeed() const;
    void SetSpeed(float speed);

    std::uint8_t GetSpeedID() const;
    void SetSpeedID(std::uint8_t speedID);

    std::uint8_t GetCapacity() const;

    std::uint8_t GetMinLevelLimit() const;
    void SetMinLevelLimit(std::uint8_t minLevelLimit);

    std::uint8_t GetMaxLevelLimit() const;
    void SetMaxLevelLimit(std::uint8_t maxLevelLimit);

    LevelCategory GetRandomLevel() const;
    void SetRandomLevel(LevelCategory random);

    MusicSortMode GetMusicSortMode() const;
    void SetMusicSortMode(MusicSortMode sort);

    MusicSortOrder GetMusicSortOrder() const;
    void SetMusicSortOrder(MusicSortOrder order);

    std::uint8_t GetMapID() const;
    void SetMapID(std::uint8_t mapID);

    std::uint8_t GetRandomizedMapID() const;
    void SetRandomizedMapID(std::uint8_t randomizedMapID);

    std::uint8_t GetEffectID() const;
    void SetEffectID(std::uint8_t effectID);

    RoomSlot& GetMaster();
    RoomSlot& GetCurrentSlot();

    RoomSlot& GetSlot(std::size_t index);

    std::unordered_set<std::uint32_t> GetAvailableMusicIDs() const;
    void UpdateFrom(const RoomInfo& room);

private:
    SessionContext& m_session;
    WaitingService& m_service;
    Gx::ResourceManager& m_resources;

    std::uint32_t m_id{};
    RoomState     m_state{};
    sf::String    m_title{};
    bool          m_locked{};
    ChartMetadata m_music{};
    Difficulty    m_difficulty{};
    GameMode      m_mode{};
    float         m_speed;
    std::uint8_t  m_minLevelLimit{};
    std::uint8_t  m_maxLevelLimit{};

    LevelCategory  m_random{};
    std::uint8_t   m_mapID{};
    std::uint8_t   m_randomizedMapID{};
    std::uint8_t   m_effectID{1};
    MusicSortMode  m_sort{};
    MusicSortOrder m_order{};

    std::array<RoomSlot, MaxCapacity> m_slots{};
};
