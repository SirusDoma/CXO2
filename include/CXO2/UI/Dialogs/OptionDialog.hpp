#pragma once

#include <CXO2/Core/Chart.hpp>
#include <CXO2/Config/GameConfig.hpp>
#include <CXO2/States/State.hpp>

#include <Genode/UI/Dialog.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/Audio//AudioMixer.hpp>

namespace Cx
{
    class OptionDialog : public Gx::Dialog
    {
    public:
        OptionDialog(Gx::AudioMixer& mixer, GameConfig& config);
        void Initialize() override;

    private:
        bool ValidateConfig();

        void OnPresented(Parent& parent, const Gx::PresentationContext& context) override;
        void OnDismissed(Parent& parent) override;

        void Update(double delta) override;

        void OnKeyPressed(const sf::Event::KeyPressed& ev) override;
        void OnKeyReleased(const sf::Event::KeyReleased& ev) override;
        void OnTextEntered(const sf::Event::TextEntered& ev) override;

        void Invalidate() override;

        Cx::State* m_parent;
        std::map<Chart::Channel, Gx::Image*> m_keyTexts, m_keyDowns;

        Gx::AudioMixer& m_mixer;
        GameConfig&     m_appConfig;
        GameConfig      m_tempConfig;
        Chart::Channel  m_keyChannel;

        bool m_initialized = false;
        bool m_keyTestEnabled = false;
    };
}
