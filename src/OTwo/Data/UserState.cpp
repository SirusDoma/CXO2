#include <OTwo/Data/UserState.hpp>

const Player &UserState::GetPlayer() const
{
    return m_player;
}

void UserState::SetPlayer(const Player &player)
{
    m_player = player;
}

PlanetType UserState::GetPlanet() const
{
    return m_planet;
}

void UserState::SetPlanet(PlanetType planet)
{
    m_planet = planet;
}

MusicHall UserState::GetMusicHall() const
{
    return m_hall;
}

void UserState::SetMusicHall(MusicHall hall)
{
    m_hall = hall;
}

unsigned int UserState::GetChannelID() const
{
    return m_channelID;
}

void UserState::SetChannelID(unsigned int channelId)
{
    m_channelID = channelId;
}

unsigned int UserState::GetRoomID() const
{
    return m_roomID;
}

void UserState::SetRoomID(unsigned int roomId)
{
    m_roomID = roomId;
}
