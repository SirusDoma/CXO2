#ifndef O2JAM_MODELS_ROOM_HPP
#define O2JAM_MODELS_ROOM_HPP

#include <OTwo/Core/Chart.hpp>
#include <OTwo/Serializable.g.hpp>
#include <OTwo/Models/Game.hpp>

#include <SFML/System/String.hpp>
#include <SFML/Graphics/Color.hpp>

#include <string>
#include <vector>

enum class KeyMode : std::size_t
{
    Three = 3,
    Five  = 5,
    Seven = 7
};

enum class RoomState
{
    Waiting,
    Playing
};

enum class SongMode
{
    Normal,
    Random
};

enum class Role : std::uint8_t
{
    Normal        = 0,
    Administrator = 1
};

struct Guild
{
    std::uint32_t ID;
    std::string   Name;
    std::uint16_t Ranking;
};

struct Player
{
    using ItemList = std::vector<std::uint32_t>;

    std::uint32_t ID;
    ::Role        Role;
    std::string   Name;
    std::int16_t  Level;
    std::uint32_t Rank;
    std::uint32_t Exp;
    std::uint32_t NextExp;
    std::uint32_t EventPoint;
    Gender        Gender;
    std::uint32_t Gem;
    std::uint32_t Cash;
    std::uint32_t Wins;
    std::uint32_t Draws;
    std::uint32_t Loses;
    ItemList      EquippedItemIDs;
    ItemList      Inventory;
    ::Guild       Guild;
};

struct ChatMessage
{
    Player Sender;
    sf::String Content;

    Player Recipient;
    // MegaphoneInfo Megaphone;
};

enum class RoomTeam
{
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H
};

struct RoomMember : Player
{
    RoomMember() = default;
    
    RoomTeam  Team  = static_cast<RoomTeam>(-1);
    sf::Color Color = sf::Color::Transparent;
    std::uint8_t Index = 0;
    bool      Ready = false;
};

struct Room
{
    std::uint32_t     ID;
    std::uint32_t     RoomMasterID  = 0;
    std::string       Title;
    ChartMetadataView ChartMetadata;
    ::Difficulty      Difficulty;
    ::GameMode        GameMode;
    ::SongMode        SongMode;
    RoomState         State;
    float             Speed;
    std::int8_t       Locked;
    std::uint32_t     Capacity      = 8;
    std::uint32_t     MinLevelLimit = 0;
    std::uint32_t     MaxLevelLimit = 0;
    RoomMember        Members[8]    = {{},{},{},{},{},{},{},{}};
    std::uint32_t     MapID         = 0;
    std::uint32_t     EffectID      = 1;

    static constexpr std::size_t MaxCapacity = sizeof(Members) / sizeof(RoomMember);

    std::string GetRoomLevelCode(const bool useNormalMode = false) const
    {
        std::string speedStr(4, '\0');
        if (Speed > 0)
        {
            if (std::fmod(Speed, 1.0f) != 0)
                speedStr.resize(std::snprintf(&speedStr[0], speedStr.size(), "%.1f", Speed));
            else
                speedStr = std::to_string(static_cast<int>(Speed));
        }
        else
            speedStr = "R";

        if (useNormalMode || SongMode == SongMode::Normal)
        {
            std::string diffName;
            switch (Difficulty)
            {
                case Difficulty::EX: diffName = "EX"; break;
                case Difficulty::NX: diffName = "NX"; break;
                case Difficulty::HX: diffName = "HX"; break;
                case Difficulty::MX: diffName = "MX"; break;
            }

            return diffName + speedStr;
        }
        else if (SongMode == SongMode::Random)
        {
            return "RX" + speedStr;
        }

        return "MX" + speedStr;
    }
};

#endif