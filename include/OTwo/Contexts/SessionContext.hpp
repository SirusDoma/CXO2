#ifndef O2JAM_SESSION_CONTEXT_HPP
#define O2JAM_SESSION_CONTEXT_HPP

#include <OTwo/Models/Planet.hpp>
#include <OTwo/Models/Room.hpp>
#include <OTwo/Metadata/Chart/ChartMetadata.hpp>

#include <Genode/System/Provider.hpp>

class SessionContext : public Gx::Provider
{
public:
    SessionContext() = default;

    const Player &GetCurrentPlayer() const;
    void SetCurrentPlayer(const Player &player);

    Planet GetPlanet() const;
    void SetPlanet(Planet planet);

    MusicHall GetMusicHall() const;
    void SetMusicHall(MusicHall hall);

    unsigned int GetChannelID() const;
    void SetChannelID(unsigned int channelId);

    const Room &GetCurrentRoom() const;
    void SetCurrentRoom(const Room &room);

    const std::vector<ChartMetadata> &GetInstalledMusic(bool rescan = false) const;

private:
    Player m_player;
    Planet m_planet = Planet::O2Planet;
    MusicHall m_hall;
    unsigned int m_channelID;
    Room m_room;

    mutable std::vector<ChartMetadata> m_installedMusicList;
};

#endif
