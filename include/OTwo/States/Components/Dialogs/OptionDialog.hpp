#ifndef O2JAM_DIALOG_OPTION_DIALOG_HPP
#define O2JAM_DIALOG_OPTION_DIALOG_HPP

#include <Genode/UI/Dialog.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/CheckBox.hpp>
#include <Genode/UI/ProgressBar.hpp>

#include <OTwo/Chart/EventChannel.hpp>
#include <OTwo/Config/GameConfig.hpp>
#include <Genode/UI/ToolTip.hpp>
#include <OTwo/States/State.hpp>

class OptionDialog : public Gx::Dialog
{
public:
    using Gx::Dialog::Dialog;

    OptionDialog(const Gx::Dialog &copy);

    void Initialize();

private:
    bool ValidateConfig();

    virtual void OnShown(Gx::Scene &scene);
    virtual void OnClose();

    virtual void Update(double delta);

    virtual void OnKeyDown(sf::Event::KeyEvent ev);
    virtual void OnKeyUp(sf::Event::KeyEvent ev);
    virtual void OnKeyType(sf::Event::TextEvent ev);

    virtual void Invalidate();

    ::State *m_parent;
    std::map<EventChannel, Gx::Image*> m_keyTexts, m_keyDowns;

    GameConfig   m_config;
    EventChannel m_keyChannel;

    bool m_keyTestEnabled;
    bool m_initialized;
};

#endif
