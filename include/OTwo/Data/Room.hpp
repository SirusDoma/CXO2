#ifndef O2JAM_ROOM_DATA_HPP
#define O2JAM_ROOM_DATA_HPP

#include <OTwo/Metadata/Chart/ChartMetadata.hpp>

#include <OTwo/Data/Game.hpp>
#include <OTwo/Data/Character.hpp>

#include <string>

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

struct RoomData
{
    unsigned int  ID;
    std::string   Title;
    ChartMetadata Chart;
    Difficulty    Difficulty;
    GameMode      GameMode;
    SongMode      SongMode;
    RoomState     State;
    float         Speed;
    bool          Locked;
    unsigned int  PlayerCount   = 1;
    unsigned int  Capacity      = 8;
    unsigned int  MinLevelLimit = 0;
    unsigned int  MaxLevelLimit = 0;
};

struct Player
{
    unsigned int ID;
    int Level;
    std::string Name;
    bool Administrator;
    Gender Gender;
    unsigned int Gem;
    unsigned int Cash;
};

struct ChatData
{
    Player Sender;
    std::string Message;

    Player Recipient;
    // MegaphoneInfo Megaphone;
};
#endif