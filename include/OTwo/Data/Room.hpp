#ifndef O2JAM_ROOM_DATA_HPP
#define O2JAM_ROOM_DATA_HPP

#include <OTwo/Chart/Chart.hpp>

#include <OTwo/Data/Game.hpp>
#include <OTwo/Data/Character.hpp>

#include <SFML/System/String.hpp>

#include <string>
#include <vector>

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

struct PlayerData
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

struct ChatData
{
    PlayerData Sender;
    sf::String Message;

    PlayerData Recipient;
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

struct RoomMember : PlayerData
{
    RoomMember() = default;
    
    RoomTeam   Team  = static_cast<RoomTeam>(-1);
    Gx::Uint32 Index = 0;
};

struct RoomData
{
    using MemberList = RoomMember[];

    Gx::Uint32        ID;
    Gx::Uint32        RoomMasterID  = 0;
    std::string       Title;
    ChartMetadataView Chart;
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
};

#endif