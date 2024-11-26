#pragma once

#include <OTwo/States/State.hpp>
#include <Genode/Audio/AudioMixer.hpp>

#include <vector>

class Avatar;
class ItemFactory;
class SessionContext;
class MusicSelectionContext;
class GameContext;
class StateWaiting7K : public State
{
public:
    StateWaiting7K(Gx::AudioMixer& mixer, SessionContext& session, GameContext& game, ItemFactory& items);
    void Initialize() override;

private:
    void OnKeyPressed(const sf::Event::KeyPressed& ev) override;
    void OnKeyReleased(const sf::Event::KeyReleased& ev) override;

    void ShowEmoticon(const Avatar* avatar, const std::string& emoticonID);

    Gx::AudioMixer& m_mixer;
    SessionContext& m_session;
    GameContext& m_game;
    ItemFactory& m_items;

    Avatar* m_playerAvatar;
    std::vector<Avatar*> m_avatars;
};
