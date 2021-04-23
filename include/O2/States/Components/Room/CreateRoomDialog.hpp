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

    void SetJamModeButton(Gx::ResourcePtr<Gx::RadioButton> jamModeButton);
    void SetVsModeButton(Gx::ResourcePtr<Gx::RadioButton> vsModeButton);
    void SetSingleModeButton(Gx::ResourcePtr<Gx::RadioButton> singleModeButton);
    void SetJamAnimation(Gx::ResourcePtr<Gx::Animation> jamAnimation);
    void SetVsModeAnimation(Gx::ResourcePtr<Gx::Animation> vsModeAnimation);
    void SetSingleModeAnimation(Gx::ResourcePtr<Gx::Animation> singleModeAnimation);
    void SetLevelLimitCheckBox(Gx::ResourcePtr<Gx::CheckBox> levelLimit);

private:
    Gx::ResourcePtr<Gx::RadioButton> m_jamModeButton, m_vsModeButton, m_singleModeButton;
    Gx::ResourcePtr<Gx::Animation>   m_jamAnimation, m_vsModeAnimation, m_singleModeAnimation;
    Gx::ResourcePtr<Gx::CheckBox>    m_levelLimitCheckBox;
};

#endif
