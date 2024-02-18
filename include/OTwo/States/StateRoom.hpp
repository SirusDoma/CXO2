#ifndef O2JAM_STATE_ROOM_HPP
#define O2JAM_STATE_ROOM_HPP

#include <Genode/Graphics.hpp>
#include <Genode/SceneGraph.hpp>
#include <Genode/Tasks/Sequence.hpp>

#include <OTwo/States/State.hpp>
#include <OTwo/Dialogs/OptionDialog.hpp>
#include <OTwo/States/Components/Common/ChatPanel.hpp>
#include <OTwo/States/Components/Room/RoomList.hpp>
#include <OTwo/States/Components/Room/UserList.hpp>

#include <OTwo/Data/Planet.hpp>
#include <OTwo/Data/Room.hpp>
#include <OTwo/Character/Item.hpp>
#include <OTwo/Character/Avatar.hpp>

class RoomButton;
class StateRoom : public State
{
public:
    StateRoom(Planet::MusicHall hall, Planet::ChannelInfo channel);
    virtual void Initialize();

private:
    virtual void Update(double delta);
    virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;

    void OnExitPlanet();

    sf::Music          *m_bgm;
    Planet::MusicHall   m_hall;
    Planet::ChannelInfo m_channel;
    Avatar             *m_avatar;
    Item               *m_item;
    RoomList            m_roomList;
    ChatPanel           m_chatPanel;
    UserList            m_userList;
    OptionDialog       *m_optionDialog;
};

#endif
