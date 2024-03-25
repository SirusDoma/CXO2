#ifndef O2JAM_ROOM_DATA_HPP
#define O2JAM_ROOM_DATA_HPP

#include <OTwo/Metadata/Chart/ChartMetadata.hpp>

#include <OTwo/Data/Game.hpp>
#include <OTwo/Data/Character.hpp>

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

enum class GameMode
{
    Single,
    Versus,
    Album,
    Couple,
    Live,
    Jam
};

struct Player
{
    unsigned int ID;
    std::string Name;
    int Level;
    Gender Gender;
    unsigned int Gem;
    unsigned int Cash;
    bool Administrator;
    std::vector<unsigned int> EquippedItemIDs;
};

enum RoomTeam
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
    RoomTeam Team;
};

struct ChatData
{
    Player Sender;
    std::string Message;

    Player Recipient;
    // MegaphoneInfo Megaphone;
};


struct RoomData
{
    using MemberList = RoomMember[];

    unsigned int  ID;
    unsigned int  RoomMasterID  = 0;
    std::string   Title;
    ChartMetadata Chart;
    Difficulty    Difficulty;
    GameMode      GameMode;
    SongMode      SongMode;
    RoomState     State;
    float         Speed;
    bool          Locked;
    unsigned int  Capacity      = 8;
    unsigned int  MinLevelLimit = 0;
    unsigned int  MaxLevelLimit = 0;
    RoomMember    Members[8]    = {{},{},{},{},{},{},{},{}};
};

#endif