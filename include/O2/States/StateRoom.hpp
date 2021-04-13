#ifndef O2JAM_STATE_ROOM_HPP
#define O2JAM_STATE_ROOM_HPP

#include <SFML/Audio.hpp>

#include <Genode/Graphics.hpp>
#include <Genode/SceneGraph.hpp>
#include <Genode/Tasks/Sequence.hpp>

#include <O2/States/State.hpp>
#include <O2/States/Components/Common/OptionDialog.hpp>
#include <O2/States/Components/Room/ChatPanel.hpp>
#include <O2/States/Components/Room/RoomList.hpp>
#include <O2/States/Components/Room/UserList.hpp>

#include <O2/Data/Planet.hpp>
#include <O2/Data/Room.hpp>

class RoomButton;
class StateRoom : public State
{
public:
    StateRoom(Planet::MusicHall hall, Planet::ChannelInfo channel);
    virtual void Initialize();

private:
    void OnExitPlanet();

    sf::Music *m_bgm;
    Planet::MusicHall m_hall;
    Planet::ChannelInfo m_channel;

    OptionDialog *m_optionDialog;

    RoomList  m_roomList;
    ChatPanel m_chatPanel;
    UserList  m_userList;
};

#endif
