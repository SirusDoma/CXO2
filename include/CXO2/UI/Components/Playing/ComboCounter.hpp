#pragma once

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/Entities/Renderable.hpp>
#include <Genode/Graphics/Animation.hpp>
#include <CXO2/UI/BitmapNumber.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Tasks/Action.hpp>
#include <Genode/Tasks/Delay.hpp>
#include <Genode/Tasks/TaskGroup.hpp>
#include <Genode/Tween/Move.hpp>

namespace Gx { class ResourceManager; }

namespace Cx
{
    class ComboCounter : public virtual Gx::Node, public Gx::Renderable, public Gx::UpdatableContainer
    {
    public:
        explicit ComboCounter(Gx::Animation* header, BitmapNumber* counter);

        void Initialize() override;

        unsigned int GetCombo() const;
        void SetCombo(unsigned int combo);

    private:
        void Update(const sf::Time& delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        Gx::Animation*    m_header;
        BitmapNumber* m_counter;
        Gx::Sequence      m_sequence;
    };
}
