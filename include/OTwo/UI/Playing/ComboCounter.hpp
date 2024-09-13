#ifndef O2JAM_PLAYING_COMBO_COUNTER_HPP
#define O2JAM_PLAYING_COMBO_COUNTER_HPP

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/Entities/Renderable.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/Tasks/Sequence.hpp>

class ComboCounter : public virtual Gx::Node, public Gx::Renderable, public Gx::UpdatableContainer
{
public:
    ComboCounter() = default;

    void Initialize() override;

    unsigned int GetCombo() const;
    void SetCombo(unsigned int combo);

private:
    void Update(const double delta) override;
    Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

    Gx::Animation* m_header;
    Gx::BitmapNumber*    m_counter;
    Gx::Sequence   m_sequence;
};

#endif
