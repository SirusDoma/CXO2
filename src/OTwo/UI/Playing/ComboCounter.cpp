#include <OTwo/UI/Playing/ComboCounter.hpp>
#include <OTwo/States/State.hpp>

#include <Genode/Tasks/Action.hpp>
#include <Genode/Tasks/Step.hpp>
#include <Genode/Tween/Move.hpp>

ComboCounter::ComboCounter(Gx::Animation* header, Gx::BitmapNumber* counter) :
    m_header(header),
    m_counter(counter),
    m_action(),
    m_headerMove(),
    m_counterMove(),
    m_delay(sf::Time::Zero)
{
    if (m_header)
        Gx::Node::AddChild(*m_header);

    if (m_counter)
        Gx::Node::AddChild(*m_counter);
}

void ComboCounter::Initialize()
{
    Gx::Node::Initialize();

    SetVisible(false);
    SetPosition(sf::Vector2f(0.f, -30.f / 5.f));

    m_action = Gx::Action([this]
    {
        if (m_header)
            m_header->Reset();

        SetVisible(true);
    });

    m_headerMove  = Gx::Move(*m_header, sf::Vector2f(m_header->GetPosition().x, m_header->GetPosition().y - 8.f), sf::seconds(1.f / 60.f * 5));
    m_counterMove = Gx::Move(*m_counter, sf::Vector2f(m_counter->GetPosition().x, m_counter->GetPosition().y - 23.f), sf::seconds(1.f / 60.f * 5));
    m_moveGroup   = Gx::TaskGroup(m_action, m_headerMove, m_counterMove);

    // Make sure initial is recorded now
    m_headerMove.Update(0);
    m_counterMove.Update(0);

    m_delay    = Gx::Delay(sf::milliseconds(1000));
    m_sequence = Gx::Sequence([this]
        {
            if (m_header)
                m_header->Stop();

            SetVisible(false);
        },
        m_moveGroup, m_delay
    );

    m_sequence.Stop();
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

void ComboCounter::Update(const double delta)
{
    m_sequence.Update(delta);
    Gx::UpdatableContainer::Update(delta);

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
