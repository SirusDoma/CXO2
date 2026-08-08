#pragma once

#include <Genode/Entities/Renderable.hpp>
#include <Genode/Entities/Updatable.hpp>
#include <Genode/Entities/Inputable.hpp>

#include <memory>

namespace Cx
{
    class State;
    class StateExtension;

    using StateExtensionPtr = std::unique_ptr<StateExtension, void(*)(StateExtension*)>;

    class StateExtension : public Gx::Renderable, public Gx::Updatable, public Gx::Inputable
    {
    public:
        ~StateExtension() override = default;

    protected:
        StateExtension() = default;

        virtual bool Initialize() { return m_next->Initialize(); }
        virtual void Finalize() { m_next->Finalize(); }

        void Update(const sf::Time& delta) override { m_next->Update(delta); }
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override { return m_next->Render(surface, states); }
        bool Input(const sf::Event& ev) override { return m_next->Input(ev); }

        State& GetState() const { return *m_state; }

    private:
        friend class State;

        State* m_state{nullptr};
        StateExtension* m_next{nullptr};
    };
}
