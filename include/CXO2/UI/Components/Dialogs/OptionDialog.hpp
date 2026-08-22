#pragma once

#include <CXO2/Core/Chart.hpp>
#include <CXO2/Config/GameConfig.hpp>
#include <CXO2/States/State.hpp>

#include <CXO2/UI/Dialog.hpp>
#include <CXO2/UI/Image.hpp>
#include <Genode/Audio//AudioMixer.hpp>

#include <unordered_map>

namespace Cx
{
    class RadioButton;
    class ToggleButton;

    class OptionDialog : public Cx::Dialog
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

        void OnGfxToggleCheckChanged(Cx::ToggleButton& sender, Control::Event& ev);
        void OnCursorToggleCheckChanged(Cx::ToggleButton& sender, Control::Event& ev);
        void OnKeyTestToggleCheckChanged(Cx::ToggleButton& sender, Control::Event& ev);
        void OnBgmToggleCheckChanged(Cx::ToggleButton& sender, Control::Event& ev);

        void OnKeyTextClicked(Control& sender, Control::Event& ev);

        void OnMasterVolumeUpButtonHeld(Control& sender, Control::Event& ev);
        void OnMasterVolumeDownButtonHeld(Control& sender, Control::Event& ev);
        void OnMusicVolumeUpButtonHeld(Control& sender, Control::Event& ev);
        void OnMusicVolumeDownButtonHeld(Control& sender, Control::Event& ev);
        void OnEffectVolumeUpButtonHeld(Control& sender, Control::Event& ev);
        void OnEffectVolumeDownButtonHeld(Control& sender, Control::Event& ev);

        void OnSaveButtonClicked(Control& sender, Control::Event& ev);
        void OnDefaultButtonClicked(Control& sender, Control::Event& ev);

        void OnKeyTabCheckChanged(Cx::RadioButton& sender, Control::Event& ev);
        void OnSoundTabCheckChanged(Cx::RadioButton& sender, Control::Event& ev);

        Cx::State* m_parent;
        std::map<Chart::Channel, Image*> m_keyTexts, m_keyDowns;
        std::unordered_map<Control*, std::pair<Chart::Channel, std::size_t>> m_keyTextChannels;

        Gx::AudioMixer& m_mixer;
        GameConfig&     m_appConfig;
        GameConfig      m_tempConfig;
        Chart::Channel  m_keyChannel;

        bool m_initialized = false;
        bool m_keyTestEnabled = false;
    };
}
