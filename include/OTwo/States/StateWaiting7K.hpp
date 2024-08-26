#ifndef O2JAM_STATE_WAITING_7K_HPP
#define O2JAM_STATE_WAITING_7K_HPP

#include <OTwo/States/State.hpp>

#include <vector>

class Avatar;
class StateWaiting7K : public State
{
public:
    explicit StateWaiting7K(State &&state);

    void Initialize() override;



private:
    void OnKeyDown(const sf::Event::KeyEvent ev) override;
    void OnKeyUp(const sf::Event::KeyEvent ev) override;

    void ShowEmoticon(const Avatar* avatar, const std::string& emoticonID);

    Avatar* m_playerAvatar;
    std::vector<Avatar*> m_avatars;
};

#endif
