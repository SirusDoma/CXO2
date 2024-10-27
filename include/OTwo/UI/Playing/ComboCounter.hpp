#pragma once

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/Entities/Renderable.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Tasks/Action.hpp>
#include <Genode/Tasks/TaskGroup.hpp>
#include <Genode/Tween/Move.hpp>

namespace Gx { class ResourceManager; }
class ComboCounter : public virtual Gx::Node, public Gx::Renderable, public Gx::UpdatableContainer
{
public:
    explicit ComboCounter(Gx::Animation* header, Gx::BitmapNumber* counter);

    void Initialize() override;

    unsigned int GetCombo() const;
    void SetCombo(unsigned int combo);

private:
    void Update(const double delta) override;
    Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

    Gx::Animation*       m_header;
    Gx::BitmapNumber*    m_counter;
    Gx::Sequence         m_sequence;
    Gx::Action           m_action;
    Gx::TaskGroup        m_moveGroup;
    Gx::Move             m_headerMove, m_counterMove;
    Gx::Delay            m_delay;
};
