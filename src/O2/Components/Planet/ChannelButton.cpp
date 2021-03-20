#include <O2/Components/Planet/ChannelButton.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <O2/IO/Metadata/UI/ButtonMetadata.hpp>

ChannelButton::ChannelButton() :
    m_button(),
    m_channelName(),
    m_selector()
{
    Initialize();
}

void ChannelButton::Initialize()
{
    m_button = Gx::ResourceManager::Instance()->Create<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Background.json");
    SetOrigin(m_button->GetOrigin());
    SetPosition(m_button->GetPosition());

    m_button->SetOrigin(0.f, 0.f);
    m_button->SetPosition(0.f, 0.f);

    m_channelName = Gx::ResourceManager::Instance()->Create<Gx::Image>("Metadata/State/Planet/ChannelBoard/Btn_Channel/ChannelName.json");
    m_selector    = Gx::ResourceManager::Instance()->Create<Gx::Image>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Selector.json");
    m_selector->SetVisible(false);

    AddChild(m_button, m_channelName, m_selector);
}

const sf::FloatRect ChannelButton::GetLocalBounds() const
{
    if (m_button)
        return m_button->GetLocalBounds();

    return sf::FloatRect();
}

void ChannelButton::SetPlanet(Planet planet)
{
    Gx::ResourceMetadata *metadata = nullptr;
    switch (planet)
    {
        case Planet::Kaliope:
            metadata = Gx::ResourceManager::Instance()->GetMetadata<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_Channel/High.json");
            m_channelName->SetFrame("Kaliope");
            break;
        case Planet::Kleo:
            m_channelName->SetFrame("Kleo");
            metadata = Gx::ResourceManager::Instance()->GetMetadata<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Intermediate.json");
            break;
        case Planet::Philix:
            m_channelName->SetFrame("Philix");
            metadata = Gx::ResourceManager::Instance()->GetMetadata<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Intermediate.json");
            break;
        case Planet::Melpomin:
            m_channelName->SetFrame("Melpomin");
            metadata = Gx::ResourceManager::Instance()->GetMetadata<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Beginner.json");
            break;
        case Planet::Thalo:
            m_channelName->SetFrame("Thalo");
            metadata = Gx::ResourceManager::Instance()->GetMetadata<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Beginner.json");
            break;
        case Planet::Euta:
            m_channelName->SetFrame("Euta");
            metadata = Gx::ResourceManager::Instance()->GetMetadata<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Beginner.json");
            break;
        default:
            metadata = Gx::ResourceManager::Instance()->GetMetadata<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Background.json");
            break;
    }

    if (!metadata)
        return;

    auto buttonData = dynamic_cast<ButtonMetadata*>(metadata);
    if (buttonData)
    {
        for (auto[state, sprite] : buttonData->States)
            m_button->SetStateFrame(state, sprite.TexCoords);
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
}


