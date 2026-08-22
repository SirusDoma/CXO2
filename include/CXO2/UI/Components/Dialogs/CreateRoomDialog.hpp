#pragma once

#include <CXO2/UI/Dialog.hpp>
#include <Genode/Audio/AudioMixer.hpp>

namespace Cx
{
    class RadioButton;
    class ToggleButton;

    enum class GameMode : std::uint8_t;

    class SessionContext;
    class CreateRoomDialog : public Cx::Dialog
    {
    public:
        CreateRoomDialog(SessionContext& session, Gx::AudioMixer& mixer);
        void Initialize() override;

        GameMode GetRoomMode() const;

        sf::String GetRoomName() const;
        std::string GetRoomPassword() const;
        unsigned int GetMinLevelLimit() const;
        unsigned int GetMaxLevelLimit() const;

    protected:
        void OnPresented(Parent& parent, const Gx::PresentationContext& context) override;
        void OnAccepted() override;

    private:
        void OnJamModeButtonClicked(Control& sender, Control::Event& ev);
        void OnVersusModeButtonClicked(Control& sender, Control::Event& ev);
        void OnSingleModeButtonClicked(Control& sender, Control::Event& ev);
        void OnJamModeButtonCheckChanged(Cx::RadioButton& sender, Control::Event& ev);
        void OnVersusModeButtonCheckChanged(Cx::RadioButton& sender, Control::Event& ev);
        void OnSingleModeButtonCheckChanged(Cx::RadioButton& sender, Control::Event& ev);

        void OnLevelLimitToggleCheckChanged(Cx::ToggleButton& sender, Control::Event& ev);

        bool m_initialized = false;
        SessionContext& m_session;
        Gx::AudioMixer& m_mixer;
    };
}
