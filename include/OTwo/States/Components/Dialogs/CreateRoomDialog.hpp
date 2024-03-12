#ifndef O2JAM_DIALOG_CREATE_ROOM_DIALOG_HPP
#define O2JAM_DIALOG_CREATE_ROOM_DIALOG_HPP

#include <Genode/UI/Dialog.hpp>
#include <Genode/UI/ToolTip.hpp>
#include <Genode/UI/CheckBox.hpp>
#include <Genode/UI/RadioButton.hpp>
#include <Genode/UI/TextBox.hpp>

#include <Genode/Graphics/Animation.hpp>

class CreateRoomDialog : public Gx::Dialog
{
public:
    using Gx::Dialog::Dialog;

    CreateRoomDialog(const Gx::Dialog &copy);

    virtual void Initialize();
    virtual void OnShown(Gx::Scene &scene);

    void SetCreateModeSoundEffect(Gx::ResourcePtr<sf::Sound> sound);

protected:
    virtual void OnAccepted();

private:
    Gx::ResourcePtr<sf::Sound>       m_sfxCreateMode;
    Gx::ResourcePtr<Gx::RadioButton> m_jamModeButton, m_vsModeButton, m_singleModeButton;
    Gx::ResourcePtr<Gx::Animation>   m_jamAnimation, m_vsModeAnimation, m_singleModeAnimation;
    Gx::ResourcePtr<Gx::CheckBox>    m_levelLimitCheckBox;
    Gx::ResourcePtr<Gx::TextBox>     m_titleTextBox, m_passwordTextBox, m_minLevelLimitTextBox, m_maxLevelLimitTextBox;
    Gx::ResourcePtr<Gx::ToolTip>     m_toolTip;

    bool m_initialized;
};

#endif
