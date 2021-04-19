#ifndef O2JAM_COMMON_OPTION_DIALOG_HPP
#define O2JAM_COMMON_OPTION_DIALOG_HPP

#include <Genode/UI/Dialog.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/CheckBox.hpp>
#include <Genode/UI/ProgressBar.hpp>

#include <O2/Chart/EventChannel.hpp>
#include <O2/Config/GameConfig.hpp>

class OptionDialog : public Gx::Dialog
{
public:
    using Gx::Dialog::Dialog;

    void Initialize(Gx::Scene &scene);

private:
    bool ValidateConfig();

    virtual void OnShown(Gx::Scene &scene);
    virtual void OnClose();

    virtual void Update(double delta);

    virtual void OnKeyDown(sf::Event::KeyEvent ev);
    virtual void OnKeyUp(sf::Event::KeyEvent ev);
    virtual void OnKeyType(sf::Event::TextEvent ev);

    virtual void Invalidate();

    Gx::Image       *m_background, *m_keySelect;
    Gx::CheckBox    *m_gfxCheckBox, *m_cursorCheckBox, *m_keyTestCheckBox, *m_bgmCheckBox;
    Gx::RadioButton *m_keyTab, *m_soundTab;
    Gx::ProgressBar *m_allVolumeBar, *m_musicVolumeBar, *m_effectVolumeBar;
    sf::Sound       *m_sfxNavigation;
    Gx::UiContainer  m_keyOptionContainer, m_soundOptionContainer;
    std::map<EventChannel, Gx::Image*> m_keyTexts, m_keyDowns;

    GameConfig   m_config;
    EventChannel m_keyChannel;

    bool m_keyTestEnabled;
};

#endif
