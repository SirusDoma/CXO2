#ifndef O2JAM_ROOM_CREATE_ROOM_DIALOG_HPP
#define O2JAM_ROOM_CREATE_ROOM_DIALOG_HPP

#include <Genode/UI/Dialog.hpp>
#include <Genode/UI/CheckBox.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/Graphics/Animation.hpp>

class CreateRoomDialog : public Gx::Dialog
{
public:
    using Gx::Dialog::Dialog;

    void Initialize(Gx::Application &app);

    void SetJamModeButton(Gx::RadioButton *jamModeButton);
    void SetVsModeButton(Gx::RadioButton *vsModeButton);
    void SetSingleModeButton(Gx::RadioButton *singleModeButton);
    void SetJamAnimation(Gx::Animation *jamAnimation);
    void SetVsModeAnimation(Gx::Animation *vsModeAnimation);
    void SetSingleModeAnimation(Gx::Animation *singleModeAnimation);
    void SetLevelLimit(Gx::CheckBox *levelLimit);

private:
    Gx::RadioButton *m_jamModeButton, *m_vsModeButton, *m_singleModeButton;
    Gx::Animation   *m_jamAnimation, *m_vsModeAnimation, *m_singleModeAnimation;
    Gx::CheckBox    *m_levelLimit;
};

#endif
