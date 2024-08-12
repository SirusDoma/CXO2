#include <OTwo/States/State.hpp>
#include <OTwo/UI/Playing/JudgementIndicator.hpp>

void JudgementIndicator::Initialize()
{
    Node::Initialize();

    const auto parent = GetParent<State>();
    if (!parent)
        return;

    m_elapsed = 0;
    for (auto& [acc, name] : std::unordered_map<Accuracy, std::string>{
            { Accuracy::Cool, "IDC_ANIMATION_NOTE_COOL" },
            { Accuracy::Good, "IDC_ANIMATION_NOTE_GOOD" },
            { Accuracy::Bad,  "IDC_ANIMATION_NOTE_BAD" },
            { Accuracy::Miss, "IDC_ANIMATION_NOTE_MISS" },
        })
    {
        if (const auto indicator = parent->FindResource<Gx::Animation>(name); indicator)
        {
            m_indicators[acc] = indicator;
            indicator->SetVisible(false);
        }
    }
}

void JudgementIndicator::Play(const Accuracy accuracy)
{
    m_elapsed = 0;
    for (auto [acc, indicator] : m_indicators)
    {
        if (acc == accuracy)
        {
            if (m_scale)
                m_scale->Stop();

            m_target = indicator;
            m_target->SetVisible(true);
            m_target->SetScale(sf::Vector2f(0.f, 0.f));
            m_scale = std::make_unique<Gx::Scale>(m_target, sf::Vector2f(1.f, 1.f), sf::milliseconds(100));
        }
        else
            indicator->SetVisible(false);
    }
}

void JudgementIndicator::Update(const double delta)
{
    if (m_scale)
    {
        m_scale->Update(delta);
        if (m_scale->GetState() == Gx::TaskState::Completed)
            m_elapsed += delta;
    }
}

Gx::RenderStates JudgementIndicator::Render(Gx::RenderSurface &surface, Gx::RenderStates states) const
{
    if (m_scale && m_target && m_target->IsVisible() && m_elapsed >= 1000)
        m_target->SetVisible(false);

    for (auto [_, indicator] : m_indicators)
        indicator->Render(surface, states);

    return states;
}
