#ifndef O2JAM_STATE_ROOM_HPP
#define O2JAM_STATE_ROOM_HPP

#include <OTwo/States/State.hpp>

#include <OTwo/UI/Dialogs/CreateRoomDialog.hpp>
#include <OTwo/UI/Dialogs/OptionDialog.hpp>

class StateRoom : public State
{
public:
    explicit StateRoom(State &&state);
    void Initialize() override;

private:
    void OnMyRoomClicked() const;
    void OnBackClicked() const;

    OptionDialog m_optionDialog;
    CreateRoomDialog m_createRoomDialog;
};

#endif
