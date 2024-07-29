#ifndef O2JAM_STATE_ROOM_HPP
#define O2JAM_STATE_ROOM_HPP

#include <OTwo/States/State.hpp>

#include <OTwo/States/Components/Dialogs/CreateRoomDialog.hpp>
#include <OTwo/States/Components/Dialogs/OptionDialog.hpp>

class StateRoom : public State
{
public:
    explicit StateRoom(State &&state);
    void Initialize() override;

private:
    void OnBackClicked();

    OptionDialog m_optionDialog;
    CreateRoomDialog m_createRoomDialog;
};

#endif
