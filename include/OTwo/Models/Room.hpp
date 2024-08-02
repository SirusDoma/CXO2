#ifndef O2JAM_MODELS_ROOM_HPP
#define O2JAM_MODELS_ROOM_HPP

#include <OTwo/Chart/Chart.hpp>

#include <OTwo/Models/Game.hpp>
#include <OTwo/Models/Character.hpp>

#include <SFML/System/String.hpp>

#include <string>
#include <vector>

enum class KeyMode
{
    Three,
    Five,
    Seven
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

struct Player
{
    using ItemList = std::vector<Gx::Uint32>;

    Gx::Uint32   ID;
    std::string  Name;
    Gx::Int32    Level;
    ::Gender     Gender;
    Gx::Uint32   Gem;
    Gx::Uint32   Cash;
    Gx::Int8     Administrator;
    ItemList     EquippedItemIDs;
    ItemList     Inventory;
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
    Gx::Uint8 Index = 0;
};

struct Room
{
    Gx::Uint32        ID;
    Gx::Uint32        RoomMasterID  = 0;
    std::string       Title;
    ChartMetadataView ChartMetadata;
    ::Difficulty      Difficulty;
    ::GameMode        GameMode;
    ::SongMode        SongMode;
    RoomState         State;
    float             Speed;
    Gx::Int8          Locked;
    Gx::Uint32        Capacity      = 8;
    Gx::Uint32        MinLevelLimit = 0;
    Gx::Uint32        MaxLevelLimit = 0;
    RoomMember        Members[8]    = {{},{},{},{},{},{},{},{}};
    Gx::Uint32        MapID         = 0;
    Gx::Uint32        EffectID      = 1;

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