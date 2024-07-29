#ifndef O2JAM_STATE_PLAYING_HPP
#define O2JAM_STATE_PLAYING_HPP

#include <OTwo/States/State.hpp>
#include <OTwo/Chart/Chart.hpp>
#include <OTwo/Config/GameConfig.hpp>
#include <OTwo/Contexts/GameContext.hpp>

#include <Genode/SceneGraph.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <Genode/UI/Image.hpp>

class StatePlaying : public State
{
public:
    StatePlaying() = default;
    explicit StatePlaying(State &&state);

    void Initialize() override;

    void Update(const double delta) override;

    void OnKeyDown(const sf::Event::KeyEvent ev) override;
    void OnKeyUp(const sf::Event::KeyEvent ev) override;


private:
    GameContext *m_context;
    GameConfig *m_config;
    std::unordered_map<Chart::ChannelType, Gx::Image*> m_keyDowns, m_keyEffects;
};


#endif
