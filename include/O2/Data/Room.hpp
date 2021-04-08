#ifndef O2JAM_ROOM_HPP
#define O2JAM_ROOM_HPP

#include <O2/Data/Game.hpp>
#include <O2/Metadata/Chart/ChartMetadata.hpp>

enum RoomState
{
    Waiting,
    Playing
};

enum GameMode
{
    Single,
    Vs,
    Album,
    Couple
};

struct RoomData
{
    bool          Available;
    unsigned int  Number;
    sf::String    Title;
    ChartMetadata Chart;
    Difficulty    Difficulty;
    GameMode      GameMode;
    SongMode      SongMode;
    RoomState     State;
    float         Speed;
    bool          Locked;
    unsigned int  PlayerCount = 1;
    unsigned int  Capacity    = 8;
};

struct PlayerInfo
{
    unsigned int PlayerID;
    int Level;
    sf::String Name;
    bool Administrator;
};

struct ChatData
{
    PlayerInfo Player;
    sf::String Message;

    PlayerInfo Receiver;
    // MegaphoneInfo Megaphone;
};

#endif