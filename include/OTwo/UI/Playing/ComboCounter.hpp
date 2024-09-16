#ifndef O2JAM_PLAYING_COMBO_COUNTER_HPP
#define O2JAM_PLAYING_COMBO_COUNTER_HPP

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/Entities/Renderable.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Tasks/Action.hpp>
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
    Gx::Move             m_move;
    Gx::Delay            m_delay;
};

#endif
