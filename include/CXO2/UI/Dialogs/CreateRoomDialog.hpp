#pragma once

#include <Genode/UI/Dialog.hpp>
#include <Genode/Audio/AudioMixer.hpp>

namespace Gx
{
    class RadioButton;
    class ToggleButton;
}

namespace Cx
{
    enum class GameMode : std::uint8_t;

    class SessionContext;
    class CreateRoomDialog : public Gx::Dialog
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
        void OnJamModeButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnVersusModeButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnSingleModeButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnJamModeButtonCheckChanged(Gx::RadioButton& sender);
        void OnVersusModeButtonCheckChanged(Gx::RadioButton& sender);
        void OnSingleModeButtonCheckChanged(Gx::RadioButton& sender);

        void OnLevelLimitToggleCheckChanged(Gx::ToggleButton& sender);

        bool m_initialized = false;
        SessionContext& m_session;
        Gx::AudioMixer& m_mixer;
    };
}
