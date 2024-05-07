#include <OTwo/States/Components/Planet/ChannelButton.hpp>
#include <OTwo/Metadata/UI/RadioButtonMetadata.hpp>
#include <OTwo/States/State.hpp>

#include <cmath>

ChannelButton::ChannelButton() :
    Gx::RadioButton(),
    m_initialized(false),
    m_hall(),
    m_population(),
    m_numberIndicator(),
    m_nameIndicator(),
    m_fullIndicator(),
    m_focusIndicator(),
    m_populationCounter(),
    m_states()
{
}

void ChannelButton::Initialize()
{
    if (!GetParent<::State>() || m_initialized)
        return;

    m_numberIndicator   = FindChild<Gx::Number>("IDC_NUMBER_CHANNEL_ID");
    m_nameIndicator     = FindChild<Gx::Image>("IDC_IMAGE_CHANNEL_NAME");
    m_fullIndicator     = FindChild<Gx::Image>("IDC_IMAGE_CHANNEL_FULL");;
    m_focusIndicator    = FindChild<Gx::Image>("IDC_IMAGE_CHANNEL_FOCUS");
    m_populationCounter = FindChild<Gx::Gauge>("IDC_GAUGE_CHANNEL_COUNTER");

    if (m_focusIndicator)
    {
        m_focusIndicator->SetVisible(false);
        m_initialized = true;
    }
}

sf::FloatRect ChannelButton::GetLocalBounds() const
{
    return RadioButton::GetLocalBounds();
}

unsigned int ChannelButton::GetChannelNumber() const
{
    if (!m_numberIndicator)
        return 0;

    return m_numberIndicator->GetValue();
}

void ChannelButton::SetChannelNumber(const unsigned int channelNumber)
{
    if (!m_numberIndicator)
        return;

    m_numberIndicator->SetValue(channelNumber);
}

unsigned int ChannelButton::GetChannelPopulation() const
{
    return m_population;
}

void ChannelButton::SetChannelPopulation(const unsigned int population)
{
    m_population = population;
    if (!m_populationCounter)
        return;

    const float percentage = static_cast<float>(population) / 20.0f;
    m_populationCounter->SetValue(std::ceil(percentage) * 20.0f);

    m_fullIndicator->SetVisible(static_cast<float>(m_population) >= m_populationCounter->GetMaximumValue());
    m_populationCounter->SetVisible(!m_fullIndicator->IsVislble());
}

MusicHall ChannelButton::GetPlanet() const
{
    return m_hall;
}

void ChannelButton::AddStateFrame(const ChannelButton::Mode mode, const Gx::Control::State state, const sf::IntRect &frame)
{
    if (const auto it = m_states.find(mode); it == m_states.end())
        m_states[mode] = std::unordered_map<State, sf::IntRect>();

    m_states[mode][state] = frame;
}

void ChannelButton::SetMusicHall(const MusicHall hall)
{
    if (m_hall == hall)
        return;

    Mode mode;
    m_hall = hall;

    // TODO: Load all and put inside a map
    switch (m_hall)
    {
        case MusicHall::Kalliope:
            mode = Mode::High;
            m_nameIndicator->SetFrame("Kalliope");
            break;
        case MusicHall::Kleo:
            mode = Mode::Intermediate;
            m_nameIndicator->SetFrame("Kleo");
            break;
        case MusicHall::Philix:
            mode = Mode::Intermediate;
            m_nameIndicator->SetFrame("Philix");
            break;
        case MusicHall::Melpomin:
            mode = Mode::Beginner;
            m_nameIndicator->SetFrame("Melpomin");
            break;
        case MusicHall::Thalo:
            mode = Mode::Beginner;
            m_nameIndicator->SetFrame("Thalo");
            break;
        case MusicHall::Euta:
            mode = Mode::Beginner;
            m_nameIndicator->SetFrame("Euta");
            break;
        default:
            return;
    }

    if (auto it = m_states.find(mode); it != m_states.end())
    {
        for (auto [state, frame] : it->second)
            SetStateFrame(state, Frame{frame});
    }
}

Gx::RenderStates ChannelButton::Render(Gx::RenderSurface &surface, Gx::RenderStates states) const
{
    return CheckBox::Render(surface, states);
}

void ChannelButton::OnControlStateChanged(Gx::Control *sender, Gx::Control::State state)
{
    if (m_focusIndicator)
        m_focusIndicator->SetVisible(state == Gx::Control::State::Hover || state == Gx::Control::State::Active);

    Control::OnControlStateChanged(sender, state);
}

void ChannelButton::Invalidate()
{
    Gx::RadioButton::Invalidate();
}
