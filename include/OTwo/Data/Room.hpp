#ifndef O2JAM_ROOM_DATA_HPP
#define O2JAM_ROOM_DATA_HPP

#include <OTwo/Metadata/Chart/ChartMetadata.hpp>

#include <OTwo/Data/Game.hpp>
#include <OTwo/Data/Character.hpp>

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

struct Player
{
    unsigned int ID;
    int Level;
    sf::String Name;
    bool Administrator;
    Gender Gender;
    unsigned int Gem;
    unsigned int Cash;
};

struct ChatData
{
    Player Sender;
    sf::String Message;

    Player Recipient;
    // MegaphoneInfo Megaphone;
};
#endif