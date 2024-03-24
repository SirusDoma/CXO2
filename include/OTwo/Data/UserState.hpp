#ifndef O2JAM_USER_STATE_HPP
#define O2JAM_USER_STATE_HPP

#include <OTwo/Data/Planet.hpp>

#include <Genode/System/Module.hpp>
#include "Room.hpp"

class UserState : public Gx::Module
{
public:
    const Player &GetCurrentPlayer() const;
    void SetCurrentPlayer(const Player &player);

    PlanetType GetPlanet() const;
    void SetPlanet(PlanetType planet);

    MusicHall GetMusicHall() const;
    void SetMusicHall(MusicHall hall);

    unsigned int GetChannelID() const;
    void SetChannelID(unsigned int channelId);

    const RoomData &GetRoomData() const;
    void SetRoomData(const RoomData &room);

private:
    Player m_player;
    PlanetType m_planet = PlanetType::O2Planet;
    MusicHall m_hall;
    unsigned int m_channelID;
    RoomData m_room;
};

#endif //O2JAM_USERSTATE_HPP
