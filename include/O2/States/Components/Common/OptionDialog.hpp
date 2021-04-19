#ifndef O2JAM_COMMON_OPTION_DIALOG_HPP
#define O2JAM_COMMON_OPTION_DIALOG_HPP

#include <Genode/UI/Dialog.hpp>
#include <Genode/UI/Image.hpp>

#include <O2/Chart/EventChannel.hpp>
#include <O2/Config/GameConfig.hpp>

class OptionDialog : public Gx::Dialog
{
public:
    using Gx::Dialog::Dialog;

    void Initialize(Gx::Scene &scene);

private:
    virtual void Update(double delta);
    virtual void OnKeyDown(sf::Event::KeyEvent ev);
    virtual void OnKeyType(sf::Event::TextEvent ev);

    Gx::Image *m_background, *m_keySelect;
    Gx::UiContainer m_keyOptionContainer, m_soundOptionContainer;
    std::map<EventChannel, Gx::Image*> m_keyTexts;

    GameConfig   m_config;
    EventChannel m_keyChannel;

    bool m_keyTestEnabled;
};

#endif
