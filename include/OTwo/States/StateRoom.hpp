#ifndef O2JAM_STATE_ROOM_HPP
#define O2JAM_STATE_ROOM_HPP

#include <OTwo/States/State.hpp>

class GameContext;
class ItemFactory;
class MusicSelectionContext;
class SessionContext;

class StateRoom : public State
{
public:
    StateRoom(Gx::Mixer& mixer, SessionContext& session, MusicSelectionContext& selection, GameContext& game, ItemFactory& items);
    void Initialize() override;

private:
    void OnKeyPressed(const sf::Event::KeyPressed& ev) override;

    void OnMusicShopButtonClicked() const;
    void OnItemShopButtonClicked() const;
    void OnMyRoomButtonClicked() const;
    void OnBulletinButtonClicked() const;
    void OnTutorialButtonClicked() const;
    void OnBackButtonClicked() const;

    Gx::Mixer& m_mixer;
    SessionContext& m_session;
    MusicSelectionContext& m_selection;
    GameContext& m_game;
    ItemFactory& m_items;
};

#endif
