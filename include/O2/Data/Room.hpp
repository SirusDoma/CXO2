#ifndef O2JAM_ROOM_HPP
#define O2JAM_ROOM_HPP

#include <O2/Data/Game.hpp>
#include <O2/Metadata/Chart/ChartMetadata.hpp>

enum RoomState
{
    Waiting,
    Playing
};

struct RoomData
{
    unsigned int  Number;
    sf::String    Title;
    ChartMetadata Chart;
    Difficulty    Difficulty;
    SpeedType     SpeedType;
    RoomState     State;
    float         Speed;
    bool          Locked;
    unsigned int  PlayerCount;
    unsigned int  Capacity;
};

#endif