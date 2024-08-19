#include <OTwo/UI/Playing/ComboCounter.hpp>
#include <OTwo/States/State.hpp>

#include <Genode/Tasks/Action.hpp>
#include <Genode/Fx/Move.hpp>

void ComboCounter::Initialize()
{
    Node::Initialize();

    const auto parent = GetParent<State>();
    if (!parent)
        return;

    if (m_header = parent->FindResource<Gx::Animation>("IDC_ANIMATION_NOTE_COMBO"); m_header)
    {
        m_header->Stop();
        AddChild(m_header);
    }

    if (m_counter = parent->FindResource<Gx::Number>("IDC_NUMBER_NOTE_COMBO"); m_counter)
    {
        m_counter->SetValue(0);
        AddChild(m_counter);
    }

    SetVisible(false);
    m_sequence = Gx::Sequence([this] ()
        {
            if (m_header)
                m_header->Stop();

            SetVisible(false);
        },
        Gx::Sequence::ListOf(
        {
            parent->Create<Gx::Action>([this]
            {
                if (m_header)
                    m_header->Reset();

                SetVisible(true);
            }),
            parent->Create<Gx::Move>(this, sf::Vector2f(0.f, -30.f), sf::seconds(1.f / 60.f * 5)),
            parent->Create<Gx::Delay>(sf::milliseconds(1000))
        })
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

Gx::RenderStates ComboCounter::Render(Gx::RenderSurface &surface, Gx::RenderStates states) const
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
