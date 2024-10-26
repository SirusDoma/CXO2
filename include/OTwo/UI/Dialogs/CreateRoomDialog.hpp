#pragma once

#include <Genode/UI/Dialog.hpp>
#include <OTwo/Models/Room.hpp>

namespace Gx { class Mixer; }
class SessionContext;
class CreateRoomDialog : public Gx::Dialog
{
public:
    CreateRoomDialog(SessionContext& session, Gx::Mixer& mixer);
    void Initialize() override;

    GameMode GetRoomMode() const;
    std::string GetRoomName() const;
    std::string GetRoomPassword() const;
    unsigned int GetMinLevelLimit() const;
    unsigned int GetMaxLevelLimit() const;

protected:
    void OnShown(Gx::Scene& scene) override;
    void OnAccepted() override;

private:
    bool m_initialized = false;
    SessionContext& m_session;
    Gx::Mixer& m_mixer;
};
