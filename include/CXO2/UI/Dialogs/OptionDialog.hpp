#pragma once

#include <CXO2/Core/Chart.hpp>
#include <CXO2/Config/GameConfig.hpp>
#include <CXO2/States/State.hpp>

#include <Genode/UI/Dialog.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/Audio//AudioMixer.hpp>

#include <unordered_map>

namespace Gx
{
    class RadioButton;
    class ToggleButton;
}

namespace Cx
{
    class OptionDialog : public Gx::Dialog
    {
    public:
        OptionDialog(Gx::AudioMixer& mixer, GameConfig& config);
        void Initialize() override;

    private:
        bool ValidateConfig();

        void AdjustMasterVolume(int offset);
        void AdjustMusicVolume(int offset);
        void AdjustEffectVolume(int offset);

        void OnPresented(Parent& parent, const Gx::PresentationContext& context) override;
        void OnDismissed(Parent& parent) override;

        void Update(const sf::Time& delta) override;

        void OnKeyPressed(const sf::Event::KeyPressed& ev) override;
        void OnKeyReleased(const sf::Event::KeyReleased& ev) override;
        void OnTextEntered(const sf::Event::TextEntered& ev) override;

        void Invalidate() override;

        void OnGfxToggleCheckChanged(Gx::ToggleButton& sender, Gx::Control::Event& ev);
        void OnCursorToggleCheckChanged(Gx::ToggleButton& sender, Gx::Control::Event& ev);
        void OnKeyTestToggleCheckChanged(Gx::ToggleButton& sender, Gx::Control::Event& ev);
        void OnBgmToggleCheckChanged(Gx::ToggleButton& sender, Gx::Control::Event& ev);

        void OnKeyTextClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnMasterVolumeUpButtonHeld(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMasterVolumeDownButtonHeld(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMusicVolumeUpButtonHeld(Gx::Control& sender, Gx::Control::Event& ev);
        void OnMusicVolumeDownButtonHeld(Gx::Control& sender, Gx::Control::Event& ev);
        void OnEffectVolumeUpButtonHeld(Gx::Control& sender, Gx::Control::Event& ev);
        void OnEffectVolumeDownButtonHeld(Gx::Control& sender, Gx::Control::Event& ev);

        void OnSaveButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnDefaultButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void OnKeyTabCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev);
        void OnSoundTabCheckChanged(Gx::RadioButton& sender, Gx::Control::Event& ev);

        Cx::State* m_parent;
        std::map<Chart::Channel, Gx::Image*> m_keyTexts, m_keyDowns;
        std::unordered_map<Gx::Control*, std::pair<Chart::Channel, std::size_t>> m_keyTextChannels;

        Gx::AudioMixer& m_mixer;
        GameConfig&     m_appConfig;
        GameConfig      m_tempConfig;
        Chart::Channel  m_keyChannel;

        bool m_initialized = false;
        bool m_keyTestEnabled = false;
    };
}
