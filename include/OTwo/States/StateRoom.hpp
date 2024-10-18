#ifndef O2JAM_STATE_ROOM_HPP
#define O2JAM_STATE_ROOM_HPP

#include <OTwo/States/State.hpp>

class ItemFactory;
class MusicSelectionContext;
class SessionContext;

class StateRoom : public State
{
public:
    StateRoom(Gx::Mixer& mixer, SessionContext& session, MusicSelectionContext& selection, ItemFactory& items);
    void Initialize() override;

private:
    void OnMusicShopClicked() const;
    void OnItemShopClicked() const;
    void OnMyRoomClicked() const;
    void OnBulletinClicked() const;
    void OnBackClicked() const;

    Gx::Mixer& m_mixer;
    SessionContext& m_session;
    MusicSelectionContext& m_selection;
    ItemFactory& m_items;
};

#endif
