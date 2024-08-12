#ifndef O2JAM_PLAYING_JUDGEMENT_INDICATOR_HPP
#define O2JAM_PLAYING_JUDGEMENT_INDICATOR_HPP

#include <OTwo/Models/Game.hpp>

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/Entities/Renderable.hpp>
#include <Genode/Entities/Updatable.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <Genode/Fx/Scale.hpp>

#include <unordered_map>

class JudgementIndicator : public Gx::Node, public Gx::Renderable, public Gx::Updatable
{
public:
    JudgementIndicator() = default;

    void Initialize() override;
    void Play(Accuracy accuracy);

private:
    void Update(const double delta) override;
    Gx::RenderStates Render(Gx::RenderSurface &surface, Gx::RenderStates states) const override;

    double m_elapsed;
    Gx::Animation* m_target;
    std::unordered_map<Accuracy, Gx::Animation*> m_indicators;
    std::unique_ptr<Gx::Scale> m_scale;
};

#endif
