#pragma once

#include <OTwo/States/State.hpp>
#include <Genode/Audio/AudioMixer.hpp>

class GameContext;
class ItemFactory;
class MusicSelectionContext;
class SessionContext;

class StateRoom : public State
{
public:
    StateRoom(Gx::AudioMixer& mixer, SessionContext& session, MusicSelectionContext& selection, GameContext& game, ItemFactory& items);
    void Initialize() override;

private:
    void OnKeyPressed(const sf::Event::KeyPressed& ev) override;

    void OnMusicShopButtonClicked() const;
    void OnItemShopButtonClicked() const;
    void OnMyRoomButtonClicked() const;
    void OnBulletinButtonClicked() const;
    void OnTutorialButtonClicked() const;
    void OnBackButtonClicked() const;

    Gx::AudioMixer& m_mixer;
    SessionContext& m_session;
    MusicSelectionContext& m_selection;
    GameContext& m_game;
    ItemFactory& m_items;
};
