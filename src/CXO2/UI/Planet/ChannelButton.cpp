#include <CXO2/UI/Planet/ChannelButton.hpp>
#include <CXO2/States/State.hpp>

#include <CXO2/Constants/Identifiers/Planet.hpp>

namespace Cx
{
    using namespace Constants::Identifiers;

    ChannelButton::ChannelButton() :
        RadioButton(),
        m_nameIndicator(),
        m_fullIndicator(),
        m_focusIndicator(),
        m_numberIndicator(),
        m_populationCounter(),
        m_states(),
        m_hall(),
        m_population(),
        m_initialized(false)
    {
    }

    void ChannelButton::Initialize()
    {
        RadioButton::Initialize();

        if (!GetParent<Cx::State>() || m_initialized)
            return;

        m_numberIndicator   = FindChild<Gx::BitmapNumber>(Resource::Planet::ChannelBoard::ChannelButton::IDC_NUMBER_CHANNEL_ID);
        m_nameIndicator     = FindChild<Gx::Image>(Resource::Planet::ChannelBoard::ChannelButton::IDC_IMAGE_CHANNEL_NAME);
        m_fullIndicator     = FindChild<Gx::Image>(Resource::Planet::ChannelBoard::ChannelButton::IDC_IMAGE_CHANNEL_FULL);
        m_focusIndicator    = FindChild<Gx::Image>(Resource::Planet::ChannelBoard::ChannelButton::IDC_IMAGE_CHANNEL_FOCUS);
        m_populationCounter = FindChild<Gx::UiContainer>(Resource::Planet::ChannelBoard::ChannelButton::IDC_CONTAINER_CHANNEL_COUNTER);

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

    // ReSharper disable once CppMemberFunctionMayBeConst
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

    void ChannelButton::SetChannelPopulation(const unsigned int population, const unsigned int maxPopulation)
    {
        m_population    = population;
        m_maxPopulation = maxPopulation;

        if (m_fullIndicator)
            m_fullIndicator->SetVisible(m_population >= maxPopulation);

        if (!m_populationCounter)
            return;

        const std::size_t partition = maxPopulation / m_populationCounter->GetChildrenCount();
        for (std::size_t i = 0; i < m_populationCounter->GetChildrenCount(); i++)
        {
            const auto child = dynamic_cast<Control*>(m_populationCounter->GetChildren()[i]);
            if (!child)
                continue;

            child->SetVisible(population > i * partition);
        }

        m_populationCounter->SetVisible(m_population < maxPopulation);
    }

    MusicHall ChannelButton::GetPlanet() const
    {
        return m_hall;
    }

    void ChannelButton::AddStateFrame(const Mode mode, const State state, const sf::IntRect& frame)
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
                SetFrame(state, Frame{frame});
        }
    }

    Gx::RenderStates ChannelButton::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
    {
        return ToggleButton::Render(surface, states);
    }

    void ChannelButton::OnControlStateChanged(Control& sender, State state)
    {
        if (m_focusIndicator)
            m_focusIndicator->SetVisible(state == State::Hover || state == State::Active);

        Control::OnControlStateChanged(sender, state);
    }
}
