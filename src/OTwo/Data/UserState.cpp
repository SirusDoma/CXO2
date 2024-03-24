#include <OTwo/Data/UserState.hpp>

const Player &UserState::GetCurrentPlayer() const
{
    return m_player;
}

void UserState::SetCurrentPlayer(const Player &player)
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

const RoomData &UserState::GetRoomData() const
{
    return m_room;
}

void UserState::SetRoomData(const RoomData &room)
{
    m_room = room;
}
