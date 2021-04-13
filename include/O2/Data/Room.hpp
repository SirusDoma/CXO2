#ifndef O2JAM_ROOM_HPP
#define O2JAM_ROOM_HPP

#include <O2/Data/Game.hpp>
#include <O2/Metadata/Chart/ChartMetadata.hpp>

namespace Room
{
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
        Vs,
        Album,
        Couple
    };

    struct RoomData
    {
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

        PlayerInfo Recipient;
        // MegaphoneInfo Megaphone;
    };
}

#endif