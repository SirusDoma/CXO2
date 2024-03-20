#ifndef O2JAM_STATE_ROOM_HPP
#define O2JAM_STATE_ROOM_HPP

#include <OTwo/States/State.hpp>

#include <OTwo/States/Components/Dialogs/CreateRoomDialog.hpp>
#include <OTwo/States/Components/Dialogs/OptionDialog.hpp>
#include <OTwo/States/Components/Common/ChatPanel.hpp>
#include <OTwo/States/Components/Room/RoomContainer.hpp>
#include <OTwo/States/Components/Room/UserList.hpp>

#include <OTwo/Data/Planet.hpp>
#include <OTwo/Data/Room.hpp>
#include <OTwo/Avatar/Item.hpp>
#include <OTwo/Avatar/Avatar.hpp>

#include <Genode/Graphics.hpp>
#include <Genode/SceneGraph.hpp>
#include <Genode/Tasks/Sequence.hpp>

class StateRoom : public State
{
public:
    explicit StateRoom(State& state);
    void Initialize() override;

private:
    void Update(double delta) override;
    sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const override;

    void OnBackClicked();

    OptionDialog m_optionDialog;
    CreateRoomDialog m_createRoomDialog;
};

#endif
