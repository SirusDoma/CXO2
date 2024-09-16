#include <OTwo/UI/Playing/ComboCounter.hpp>
#include <OTwo/States/State.hpp>

#include <Genode/Tasks/Action.hpp>
#include <Genode/Fx/Move.hpp>

ComboCounter::ComboCounter(Gx::Animation* header, Gx::BitmapNumber* counter) :
    m_header(header),
    m_counter(counter),
    m_action(),
    m_move(),
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

    m_move  = Gx::Move(*this, sf::Vector2f(0.f, -30.f), sf::seconds(1.f / 60.f * 6));
    m_delay = Gx::Delay(sf::milliseconds(1000));

    m_sequence = Gx::Sequence([this]
        {
            if (m_header)
                m_header->Stop();

            SetVisible(false);
        },
        m_action, m_move, m_delay
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
