#ifndef O2JAM_STATE_PLAYING_7K_HPP
#define O2JAM_STATE_PLAYING_7K_HPP

#include <OTwo/States/State.hpp>
#include <OTwo/Chart/Chart.hpp>
#include <OTwo/Config/GameConfig.hpp>
#include <OTwo/Contexts/GameContext.hpp>

#include <Genode/SceneGraph.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <Genode/UI/Image.hpp>

class StatePlaying7K : public State
{
public:
    StatePlaying7K() = default;
    explicit StatePlaying7K(State &&state);

    void Initialize() override;

    unsigned int GetViewport() const;
    void SetViewport(unsigned int viewport);

    void Update(const double delta) override;

    void OnKeyDown(const sf::Event::KeyEvent ev) override;
    void OnKeyUp(const sf::Event::KeyEvent ev) override;

private:
    const GameContext *PrepareContext() const;

    using ImageMap = std::unordered_map<Chart::ChannelType, Gx::Image*>;
    using AnimationMap = std::unordered_map<Chart::ChannelType, Gx::Animation*>;

    const GameContext *m_context;
    const GameConfig *m_config;

    AnimationMap m_noteClicks;
    ImageMap m_keyDowns, m_keyEffects;

    unsigned int m_viewport;
};


#endif
