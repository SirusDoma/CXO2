#ifndef O2JAM_STATE_ROOM_HPP
#define O2JAM_STATE_ROOM_HPP

#include <SFML/Audio.hpp>

#include <Genode/Graphics.hpp>
#include <Genode/SceneGraph.hpp>
#include <Genode/Tasks/Sequence.hpp>

#include <O2/States/State.hpp>
#include <O2/States/Components/Common/ChatWindow.hpp>
#include <O2/States/Components/Room/ChatPanel.hpp>

#include <O2/Data/Planet.hpp>
#include <O2/Data/Room.hpp>

class RoomButton;
class StateRoom : public State
{
public:
    StateRoom(Planet planet, ChannelInfo channel);
    virtual void Initialize();

private:
    void OnExitPlanet();

    sf::Music *m_bgm;
    Planet m_planet;
    ChannelInfo m_channel;
    ChatPanel m_chatPanel;
    std::vector<std::unique_ptr<RoomButton>> m_roomButtons;
};

#endif
