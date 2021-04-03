#ifndef O2JAM_ROOM_HPP
#define O2JAM_ROOM_HPP

#include <O2/Data/Game.hpp>

enum RoomState
{
    Waiting,
    Playing
};

struct RoomData
{
    unsigned int Number;
    sf::String   Title;
    RoomState    State;
    Difficulty   Difficulty;
    SpeedType    SpeedType;
    float        Speed;
    bool         Locked;
    unsigned int PlayerCount;
    unsigned int Capacity;
};

#endif