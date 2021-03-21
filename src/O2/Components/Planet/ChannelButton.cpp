#include <O2/Components/Planet/ChannelButton.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <O2/IO/Metadata/UI/RadioButtonMetadata.hpp>

ChannelButton::ChannelButton(const Gx::RadioButton &copy) :
    Gx::RadioButton(copy)
{
    m_channelName = Gx::ResourceManager::Instance()->Create<Gx::Image>("Metadata/State/Planet/ChannelBoard/Btn_Channel/ChannelName.json");
    m_selector    = Gx::ResourceManager::Instance()->Create<Gx::Image>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Selector.json");
    m_selector->SetVisible(false);

    AddChild(m_channelName, m_selector);
}

ChannelButton::~ChannelButton()
{
}

const sf::FloatRect ChannelButton::GetLocalBounds() const
{
    return RadioButton::GetLocalBounds();
}

void ChannelButton::SetPlanet(Planet planet)
{
    Gx::ResourceMetadata *metadata = nullptr;
    switch (planet)
    {
        case Planet::Kaliope:
            metadata = Gx::ResourceManager::Instance()->GetMetadata<Gx::RadioButton>("Metadata/State/Planet/ChannelBoard/Btn_Channel/High.json");
            m_channelName->SetFrame("Kaliope");
            break;
        case Planet::Kleo:
            m_channelName->SetFrame("Kleo");
            metadata = Gx::ResourceManager::Instance()->GetMetadata<Gx::RadioButton>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Intermediate.json");
            break;
        case Planet::Philix:
            m_channelName->SetFrame("Philix");
            metadata = Gx::ResourceManager::Instance()->GetMetadata<Gx::RadioButton>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Intermediate.json");
            break;
        case Planet::Melpomin:
            m_channelName->SetFrame("Melpomin");
            metadata = Gx::ResourceManager::Instance()->GetMetadata<Gx::RadioButton>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Beginner.json");
            break;
        case Planet::Thalo:
            m_channelName->SetFrame("Thalo");
            metadata = Gx::ResourceManager::Instance()->GetMetadata<Gx::RadioButton>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Beginner.json");
            break;
        case Planet::Euta:
            m_channelName->SetFrame("Euta");
            metadata = Gx::ResourceManager::Instance()->GetMetadata<Gx::RadioButton>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Beginner.json");
            break;
        default:
            metadata = Gx::ResourceManager::Instance()->GetMetadata<Gx::RadioButton>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Background.json");
            break;
    }

    if (!metadata)
        return;

    auto radioData = dynamic_cast<RadioButtonMetadata*>(metadata);
    if (radioData)
    {
        for (auto[state, sprite] : radioData->States)
            SetStateFrame(state, sprite.TexCoords);
    }
}

void ChannelButton::OnControlStateChanged(Gx::Control *sender, Gx::Control::State state)
{
    if (m_selector)
        m_selector->SetVisible(state == Gx::Control::Hover || state == Gx::Control::Active);

    Control::OnControlStateChanged(sender, state);
}

void ChannelButton::Invalidate()
{
    Gx::RadioButton::Invalidate();
}



