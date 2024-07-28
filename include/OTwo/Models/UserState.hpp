#ifndef O2JAM_USER_STATE_HPP
#define O2JAM_USER_STATE_HPP

#include <OTwo/Models/Planet.hpp>
#include <OTwo/Models/Room.hpp>
#include <OTwo/Metadata/Chart/ChartMetadata.hpp>

#include <Genode/System/Module.hpp>

class UserState : public Gx::Module
{
public:
    UserState() = default;

    const PlayerData &GetCurrentPlayer() const;
    void SetCurrentPlayer(const PlayerData &player);

    Planet GetPlanet() const;
    void SetPlanet(Planet planet);

    MusicHall GetMusicHall() const;
    void SetMusicHall(MusicHall hall);

    unsigned int GetChannelID() const;
    void SetChannelID(unsigned int channelId);

    const RoomData &GetCurrentRoom() const;
    void SetCurrentRoom(const RoomData &room);

    const std::vector<ChartMetadata> &GetInstalledMusic() const;

private:
    PlayerData m_player;
    Planet m_planet = Planet::O2Planet;
    MusicHall m_hall;
    unsigned int m_channelID;
    RoomData m_room;

    mutable std::vector<ChartMetadata> m_installedMusicList;
};

#endif
