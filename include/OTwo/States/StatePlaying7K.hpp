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
#include <Genode/UI/TextBox.hpp>

class SessionContext;
class ItemFactory;
class Avatar;
class StatePlaying7K : public State
{
public:
    StatePlaying7K(SessionContext& session, GameContext& context, GameConfig& config, ScoreTracker& scoreTracker, LifeSystem& lifeSystem, ItemFactory& items);
    void Initialize() override;

    unsigned int GetViewport() const;
    void SetViewport(unsigned int viewport);

    void OnKeyPressed(const sf::Event::KeyPressed& ev) override;
    void OnKeyReleased(const sf::Event::KeyReleased& ev) override;

private:
    Gx::RenderStates Render(Gx::RenderSurface &surface, Gx::RenderStates states) const override;
    void Update(const double delta) override;

    void OnRenderComplete();
    void CaptureScreen();

    using ImageMap = std::unordered_map<Chart::Channel, Gx::Image*>;
    using AnimationMap = std::unordered_map<Chart::Channel, Gx::Animation*>;
    using AvatarMap = std::unordered_map<unsigned int, Avatar*>;

    SessionContext& m_session;
    GameContext& m_context;
    GameConfig& m_config;
    ScoreTracker& m_scoreTracker;
    LifeSystem& m_lifeSystem;
    ItemFactory& m_items;

    ChartRenderer m_renderer;
    AvatarMap m_avatars;
    AnimationMap m_noteClicks;
    AnimationMap m_longNoteEffects;
    Avatar* m_self;

    Gx::TextBox* m_chatBox;
    ImageMap m_keyDowns, m_keyEffects;

    unsigned int m_viewport;
};


#endif
