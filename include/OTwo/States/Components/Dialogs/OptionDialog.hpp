#ifndef O2JAM_DIALOG_OPTION_DIALOG_HPP
#define O2JAM_DIALOG_OPTION_DIALOG_HPP

#include <Genode/UI/Dialog.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/CheckBox.hpp>
#include <Genode/UI/Gauge.hpp>

#include <OTwo/Chart/Chart.hpp>
#include <OTwo/Config/GameConfig.hpp>
#include <Genode/UI/ToolTip.hpp>
#include <OTwo/States/State.hpp>

class OptionDialog : public Gx::Dialog
{
public:
    using Gx::Dialog::Dialog;

    explicit OptionDialog(const Gx::Dialog &copy);

    void Initialize() override;

private:
    bool ValidateConfig();

    void OnShown(Gx::Scene &scene) override;
    void OnClose() override;

    void Update(double delta) override;

    void OnKeyDown(sf::Event::KeyEvent ev) override;
    void OnKeyUp(sf::Event::KeyEvent ev) override;
    void OnKeyType(sf::Event::TextEvent ev) override;

    void Invalidate() override;

    ::State *m_parent;
    std::map<Chart::Channel, Gx::Image*> m_keyTexts, m_keyDowns;

    GameConfig         m_config;
    Chart::Channel m_keyChannel;

    bool m_keyTestEnabled;
    bool m_initialized;
};

#endif
