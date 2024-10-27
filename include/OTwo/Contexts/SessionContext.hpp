#pragma once

#include <OTwo/Models/Planet.hpp>
#include <OTwo/Models/Room.hpp>
#include <OTwo/Metadata/Chart/ChartMetadata.hpp>

struct ScoreResultItem
{
    RoomMember Member;
    unsigned long long Cool;
    unsigned long long Good;
    unsigned long long Bad;
    unsigned long long Miss;
    unsigned long long MaxCombo;
    unsigned long long MaxJamCombo;
    unsigned long long ScorePoint;
};

class SessionContext
{
public:
    SessionContext() = default;
    explicit SessionContext(const Player& player);

    Player& GetCurrentPlayer();

    Planet GetPlanet() const;
    void SetPlanet(Planet planet);

    MusicHall GetMusicHall() const;
    void SetMusicHall(MusicHall hall);

    unsigned int GetChannelID() const;
    void SetChannelID(unsigned int channelId);

    const Room& GetCurrentRoom() const;
    void SetCurrentRoom(const Room& room);

    const std::array<ScoreResultItem, 8>& GetLatestScoreResults() const;
    void SetLatestScoreResults(const std::array<ScoreResultItem, 8>& result);

    const std::vector<ChartMetadata> &GetInstalledMusic(bool rescan = false) const;

    void Load();
    void Save() const;

private:
    Player m_player;
    Planet m_planet = Planet::O2Planet;
    MusicHall m_hall;
    unsigned int m_channelID;
    Room m_room;

    std::array<ScoreResultItem, 8> m_lastResult;
    mutable std::vector<ChartMetadata> m_installedMusicList;
};
