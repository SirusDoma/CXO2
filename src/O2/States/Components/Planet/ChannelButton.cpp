#include <O2/States/Components/Planet/ChannelButton.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <O2/Metadata/UI/RadioButtonMetadata.hpp>

ChannelButton::ChannelButton(Gx::Scene &scene, const Gx::RadioButton &copy) :
    Gx::RadioButton(copy),
    m_scene(&scene),
    m_population()
{
    m_channelName    = m_scene->Create<Gx::Image>("Metadata/State/Planet/ChannelBoard/Btn_Channel/ChannelName.json");
    m_channelNumber  = m_scene->Create<Gx::Number>("Metadata/State/Planet/ChannelBoard/Btn_Channel/ChannelNumber.json");
    m_channelNumber->SetDigitCount(2);
    m_channelCounter = m_scene->Create<Gx::ProgressBar>("Metadata/State/Planet/ChannelBoard/Btn_Channel/ChannelCount.json");
    m_channelFull    = m_scene->Create<Gx::Image>("Metadata/State/Planet/ChannelBoard/Btn_Channel/ChannelFull.json");
    m_channelFull->SetVisible(false);
    m_hover          = m_scene->Create<Gx::Image>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Hover.json");
    m_hover->SetVisible(false);

    AddChild(m_channelNumber, m_channelName, m_channelCounter, m_channelFull, m_hover);
}

const sf::FloatRect ChannelButton::GetLocalBounds() const
{
    return RadioButton::GetLocalBounds();
}

int ChannelButton::GetChannelNumber() const
{
    return m_channelNumber->GetValue();
}

void ChannelButton::SetChannelNumber(int channelNumber)
{
    m_channelNumber->SetValue(channelNumber);
}

int ChannelButton::GetChannelPopulation() const
{
    return static_cast<int>(m_channelCounter->GetValue());
}

void ChannelButton::SetChannelPopulation(int population)
{
    m_population = population;

    float percentage = static_cast<float>(population) / 20.0f;
    m_channelCounter->SetValue(static_cast<int>(std::ceil(percentage) * 20.0f));

    m_channelFull->SetVisible(m_population >= m_channelCounter->GetMaximumValue());
    m_channelCounter->SetVisible(!m_channelFull->IsVislble());
}

Planet ChannelButton::GetPlanet() const
{
    return m_planet;
}

void ChannelButton::SetPlanet(Planet planet)
{
    if (m_planet == planet)
        return;

    Gx::ResourceMetadata *metadata = nullptr;
    m_planet = planet;

    // TODO: Load all and put inside a map
    switch (m_planet)
    {
        case Planet::Kaliope:
            metadata = m_scene->GetLocalResources().LoadMetadata<Gx::RadioButton>("Metadata/State/Planet/ChannelBoard/Btn_Channel/High.json");
            m_channelName->SetFrame("Kaliope");
            break;
        case Planet::Kleo:
            m_channelName->SetFrame("Kleo");
            metadata = m_scene->GetLocalResources().LoadMetadata<Gx::RadioButton>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Intermediate.json");
            break;
        case Planet::Philix:
            m_channelName->SetFrame("Philix");
            metadata = m_scene->GetLocalResources().LoadMetadata<Gx::RadioButton>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Intermediate.json");
            break;
        case Planet::Melpomin:
            m_channelName->SetFrame("Melpomin");
            metadata = m_scene->GetLocalResources().LoadMetadata<Gx::RadioButton>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Beginner.json");
            break;
        case Planet::Thalo:
            m_channelName->SetFrame("Thalo");
            metadata = m_scene->GetLocalResources().LoadMetadata<Gx::RadioButton>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Beginner.json");
            break;
        case Planet::Euta:
            m_channelName->SetFrame("Euta");
            metadata = m_scene->GetLocalResources().LoadMetadata<Gx::RadioButton>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Beginner.json");
            break;
        default:
            metadata = m_scene->GetLocalResources().LoadMetadata<Gx::RadioButton>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Background.json");
            break;
    }

    if (!metadata)
        return;

    auto radioData = dynamic_cast<RadioButtonMetadata*>(metadata);
    if (radioData)
    {
        for (auto[state, sprite] : radioData->GetStates())
            SetStateFrame(state, sprite.GetTexCoords());
    }
}

sf::RenderStates ChannelButton::Render(sf::RenderTarget &target, sf::RenderStates states) const
{
    return CheckBox::Render(target, states);
}

void ChannelButton::OnControlStateChanged(Gx::Control *sender, Gx::Control::State state)
{
    if (m_hover)
        m_hover->SetVisible(state == Gx::Control::Hover || state == Gx::Control::Active);

    Control::OnControlStateChanged(sender, state);
}

void ChannelButton::Invalidate()
{
    Gx::RadioButton::Invalidate();
}
