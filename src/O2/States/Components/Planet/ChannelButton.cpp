#include <O2/States/Components/Planet/ChannelButton.hpp>

#include <Genode/SceneGraph/Scene.hpp>
#include <O2/Metadata/UI/RadioButtonMetadata.hpp>

ChannelButton::ChannelButton() :
    Gx::RadioButton(),
    m_hall(),
    m_population()
{
}

void ChannelButton::Initialize()
{
    auto scene     = dynamic_cast<Gx::Scene*>(GetRoot());
    auto resources = &scene->GetLocalResources();

    m_channelName    = resources->Resolve<Gx::Image>("Interface/Metadata/State/Planet/ChannelBoard/Btn_Channel/ChannelName.json");
    m_channelNumber  = resources->Resolve<Gx::Number>("Interface/Metadata/State/Planet/ChannelBoard/Btn_Channel/ChannelNumber.json");
    m_channelCounter = resources->Resolve<Gx::ProgressBar>("Interface/Metadata/State/Planet/ChannelBoard/Btn_Channel/ChannelCount.json");
    m_channelFull    = resources->Resolve<Gx::Image>("Interface/Metadata/State/Planet/ChannelBoard/Btn_Channel/ChannelFull.json");
    m_hover          = resources->Resolve<Gx::Image>("Interface/Metadata/State/Planet/ChannelBoard/Btn_Channel/Hover.json");

    m_hover->SetVisible(false);

    AddChild(m_channelName.get(), m_channelNumber.get(), m_channelCounter.get(), m_channelFull.get(), m_hover.get());
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

Planet::MusicHall ChannelButton::GetPlanet() const
{
    return m_hall;
}

void ChannelButton::SetPlanet(Planet::MusicHall hall)
{
    if (m_hall == hall)
        return;

    const Gx::ResourceMetadata *metadata = nullptr;
    m_hall = hall;

    // TODO: Load all and put inside a map
    switch (m_hall)
    {
        case Planet::MusicHall::Kaliope:
            metadata = m_highMetadata;
            m_channelName->SetFrame("Kaliope");
            break;
        case Planet::MusicHall::Kleo:
            m_channelName->SetFrame("Kleo");
            metadata = m_intermediateMetadata;
            break;
        case Planet::MusicHall::Philix:
            m_channelName->SetFrame("Philix");
            metadata = m_intermediateMetadata;
            break;
        case Planet::MusicHall::Melpomin:
            m_channelName->SetFrame("Melpomin");
            metadata = m_beginnerMetadata;
            break;
        case Planet::MusicHall::Thalo:
            m_channelName->SetFrame("Thalo");
            metadata = m_beginnerMetadata;
            break;
        case Planet::MusicHall::Euta:
            m_channelName->SetFrame("Euta");
            metadata = m_beginnerMetadata;
            break;
        default:
            metadata = m_defaultMetadata;
            break;
    }

    if (!metadata)
        return;

    auto radioData = dynamic_cast<const RadioButtonMetadata*>(metadata);
    if (radioData)
    {
        for (auto [state, sprite] : radioData->States)
            SetStateFrame(state, sprite.TexCoords);
    }
}

void ChannelButton::SetDefaultMetadata(const Gx::ResourceMetadata *metadata)
{
    m_defaultMetadata = metadata;
}

void ChannelButton::SetHighMetadata(const Gx::ResourceMetadata *highMetadata)
{
    m_highMetadata = highMetadata;
}

void ChannelButton::SetIntermediateMetadata(const Gx::ResourceMetadata *intermediateMetadata)
{
    m_intermediateMetadata = intermediateMetadata;
}

void ChannelButton::SetBeginnerMetadata(const Gx::ResourceMetadata *beginnerMetadata)
{
    m_beginnerMetadata = beginnerMetadata;
}

sf::RenderStates ChannelButton::Render(sf::RenderTarget &target, sf::RenderStates states) const
{
    return CheckBox::Render(target, states);
}

void ChannelButton::OnControlStateChanged(Gx::Control *sender, Gx::Control::State state)
{
    if (m_hover)
        m_hover->SetVisible(state == Gx::Control::State::Hover || state == Gx::Control::State::Active);

    Control::OnControlStateChanged(sender, state);
}

void ChannelButton::Invalidate()
{
    Gx::RadioButton::Invalidate();
}
