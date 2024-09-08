#ifndef O2JAM_DIALOG_OPTION_DIALOG_HPP
#define O2JAM_DIALOG_OPTION_DIALOG_HPP

#include <OTwo/Core/Chart.hpp>
#include <OTwo/Config/GameConfig.hpp>
#include <OTwo/States/State.hpp>

#include <Genode/UI/Dialog.hpp>
#include <Genode/UI/Image.hpp>

class OptionDialog : public Gx::Dialog
{
public:
    OptionDialog(Gx::Mixer& mixer, GameConfig& config);
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

    Gx::Mixer&     m_mixer;
    GameConfig&    m_appConfig;
    GameConfig     m_tempConfig;
    Chart::Channel m_keyChannel;

    bool m_initialized = false;
    bool m_keyTestEnabled = false;
};

#endif
