#include <CXO2/UI/Playing/ComboCounter.hpp>
#include <CXO2/States/State.hpp>

#include <Genode/Tasks/Action.hpp>
#include <Genode/Tasks/Scheduler.hpp>
#include <Genode/Tween/Move.hpp>

namespace Cx
{
    ComboCounter::ComboCounter(Gx::Animation* header, Gx::BitmapNumber* counter) :
        m_header(header),
        m_counter(counter)
    {
        if (m_header)
            AddChild(*m_header);

        if (m_counter)
            AddChild(*m_counter);
    }

    void ComboCounter::Initialize()
    {
        Node::Initialize();

        SetPosition(sf::Vector2f(0.f, -30.f / 5.f));
        m_sequence = Gx::Sequence([this]
            {
                if (m_header)
                    m_header->Stop();

                SetVisible(false);
            },
            Gx::TaskGroup
            (
                Gx::Action([this]
                {
                    if (m_header)
                        m_header->Reset();

                    SetVisible(true);
                }),
                Gx::Move(*m_header, sf::Vector2f(m_header->GetPosition().x, m_header->GetPosition().y - 8.f), sf::seconds(1.f / 60.f * 5)),
                Gx::Move(*m_counter, sf::Vector2f(m_counter->GetPosition().x, m_counter->GetPosition().y - 23.f), sf::seconds(1.f / 60.f * 5))
            ),
            Gx::Delay(sf::milliseconds(1000))
        );

        // Make sure initial position is recorded now
        m_sequence.Update(sf::Time::Zero);
        m_sequence.Stop();

        SetVisible(false);
    }

    unsigned int ComboCounter::GetCombo() const
    {
        if (m_counter)
            return m_counter->GetValue();

        return 0;
    }

    void ComboCounter::SetCombo(const unsigned int combo)
    {
        if (m_counter)
            m_counter->SetValue(combo);

        if (combo > 0)
        {
            SetVisible(true);
            m_sequence.Reset();
        }
        else
        {
            SetVisible(false);
            m_sequence.Complete();
        }
    }

    void ComboCounter::Update(const sf::Time& delta)
    {
        m_sequence.Update(delta);
        UpdatableContainer::Update(delta);

    }

    Gx::RenderStates ComboCounter::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
    {
        if (!IsVisible())
            return states;

        states.transform *= GetTransform();
        if (m_header)
            surface.Render(*m_header, states);

        if (m_counter)
            surface.Render(*m_counter, states);

        return states;
    }
}
