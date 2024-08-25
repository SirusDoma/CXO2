#ifndef O2JAM_STATE_PLAYING_7K_HPP
#define O2JAM_STATE_PLAYING_7K_HPP

#include <OTwo/States/State.hpp>
#include <OTwo/Core/Chart.hpp>
#include <OTwo/Core/ChartRenderer.hpp>
#include <OTwo/Config/GameConfig.hpp>
#include <OTwo/Contexts/GameContext.hpp>

#include <Genode/SceneGraph.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <Genode/UI/Image.hpp>

class Avatar;
class StatePlaying7K : public State
{
public:
    StatePlaying7K();
    explicit StatePlaying7K(State &&state);

    void Initialize() override;

    unsigned int GetViewport() const;
    void SetViewport(unsigned int viewport);

    void OnKeyDown(const sf::Event::KeyEvent ev) override;
    void OnKeyUp(const sf::Event::KeyEvent ev) override;

private:
    Gx::RenderStates Render(Gx::RenderSurface &surface, Gx::RenderStates states) const override;
    void Update(const double delta) override;

    void OnRenderComplete();

    const GameContext *PrepareContext() const;
    void CaptureScreen();

    using ImageMap = std::unordered_map<Chart::Channel, Gx::Image*>;
    using AnimationMap = std::unordered_map<Chart::Channel, Gx::Animation*>;
    using AvatarMap = std::unordered_map<unsigned int, Avatar*>;

    ChartRenderer m_renderer;
    AvatarMap m_avatars;
    AnimationMap m_noteClicks;
    AnimationMap m_longNoteEffects;
    Avatar* m_self;

    const GameContext* m_context;
    GameConfig* m_config;
    ImageMap m_keyDowns, m_keyEffects;

    unsigned int m_viewport;
};


#endif
