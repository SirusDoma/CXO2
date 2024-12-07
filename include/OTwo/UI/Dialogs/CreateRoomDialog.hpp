#pragma once


#include <OTwo/Models/Room.hpp>

#include <Genode/UI/Dialog.hpp>
#include <Genode/Audio/AudioMixer.hpp>

class SessionContext;
class CreateRoomDialog : public Gx::Dialog
{
public:
    CreateRoomDialog(SessionContext& session, Gx::AudioMixer& mixer);
    void Initialize() override;

    GameMode GetRoomMode() const;
    std::string GetRoomName() const;
    std::string GetRoomPassword() const;
    unsigned int GetMinLevelLimit() const;
    unsigned int GetMaxLevelLimit() const;

protected:
    void OnPresented(Parent& parent, const Gx::PresentationContext& context) override;
    void OnAccepted() override;

private:
    bool m_initialized = false;
    SessionContext& m_session;
    Gx::AudioMixer& m_mixer;
};
