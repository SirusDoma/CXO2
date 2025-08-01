#pragma once

#include <OTwo/Models/Planet.hpp>
#include <OTwo/Models/Character.hpp>
#include <OTwo/Models/Room.hpp>
#include <OTwo/Metadata/Chart/O2JamChartMetadata.hpp>

class SessionContext
{
public:
    SessionContext() = default;
    explicit SessionContext(const std::string& token);

    const std::string& GetToken() const;

    CharacterInfo& GetCharacterInfo();
    void SetCharacterInfo(const CharacterInfo& CharacterInfo);

    Planet GetPlanet() const;
    void SetPlanet(Planet planet);

    MusicHall GetMusicHall() const;
    void SetMusicHall(MusicHall hall);

    unsigned int GetChannelID() const;
    void SetChannelID(unsigned int channelId);

    const std::vector<ChartMetadata>& GetInstalledMusic(bool rescan = false) const;

    void Load();
    void Save() const;

private:
    std::string m_token;

    CharacterInfo m_characterInfo;
    Planet m_planet = Planet::O2Planet;
    MusicHall m_server;
    unsigned int m_channelID;

    mutable std::vector<ChartMetadata> m_installedMusicList;
};
