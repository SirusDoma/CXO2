#ifndef O2JAM_CHART_RENDERER_HPP
#define O2JAM_CHART_RENDERER_HPP

#include <OTwo/States/State.hpp>
#include <OTwo/Chart/Chart.hpp>
#include <OTwo/Contexts/GameContext.hpp>

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/Entities/Updatable.hpp>
#include <Genode/Graphics/Animation.hpp>

#include <unordered_map>


class ChartRenderer : public Gx::Updatable
{
public:
    explicit ChartRenderer(State &state, std::initializer_list<Chart::Channel> instantiables = {});

    void Initialize(const GameContext &context);

    void Update(double delta) override;

private:
    using TemplateMap = std::unordered_map<Chart::NoteType, std::vector<Gx::Animation*>>;

    State *m_parent;
    const GameContext *m_context;

    std::vector<Chart::Channel> m_instantiables;
    std::unordered_map<Chart::Channel, TemplateMap> m_templates;
};

#endif
