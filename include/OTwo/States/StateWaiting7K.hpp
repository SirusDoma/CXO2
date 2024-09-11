#ifndef O2JAM_STATE_WAITING_7K_HPP
#define O2JAM_STATE_WAITING_7K_HPP

#include <OTwo/States/State.hpp>

#include <vector>

class Avatar;
class ItemFactory;
class SessionContext;
class MusicSelectionContext;
class StateWaiting7K : public State
{
public:
    StateWaiting7K(Gx::Mixer& mixer, SessionContext& session, ItemFactory& items);
    void Initialize() override;

private:
    void OnKeyPressed(const sf::Event::KeyPressed& ev) override;
    void OnKeyReleased(const sf::Event::KeyReleased& ev) override;

    void ShowEmoticon(const Avatar* avatar, const std::string& emoticonID);

    Gx::Mixer& m_mixer;
    SessionContext& m_session;
    ItemFactory& m_items;

    Avatar* m_playerAvatar;
    std::vector<Avatar*> m_avatars;
};

#endif
