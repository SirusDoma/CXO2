#ifndef O2JAM_STATE_PLAYING_HPP
#define O2JAM_STATE_PLAYING_HPP

#include <OTwo/States/State.hpp>
#include <Genode/SceneGraph.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <OTwo/Chart/Chart.hpp>
#include <OTwo/Config/GameConfig.hpp>

class StatePlaying : public State
{
public:
    StatePlaying() = default;
    explicit StatePlaying(State &state);

    void Initialize() override;

    void OnKeyDown(const sf::Event::KeyEvent ev) override;
    void OnKeyUp(const sf::Event::KeyEvent ev) override;


private:
    GameConfig *m_config;
};


#endif
