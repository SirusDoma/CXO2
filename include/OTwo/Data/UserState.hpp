#ifndef O2JAM_USER_STATE_HPP
#define O2JAM_USER_STATE_HPP

#include <OTwo/Data/Planet.hpp>
#include <OTwo/Data/Room.hpp>
#include <OTwo/Metadata/Chart/O2ChartMetadata.hpp>

#include <Genode/System/Module.hpp>

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

    const std::vector<O2ChartMetadata> &GetMusicList() const;
    void SetMusicList(const std::vector<O2ChartMetadata> &musicList);

private:
    Player m_player;
    PlanetType m_planet = PlanetType::O2Planet;
    MusicHall m_hall;
    unsigned int m_channelID;
    RoomData m_room;
    std::vector<O2ChartMetadata> m_musicList;
};

#endif
