#include <O2/Components/Planet/ChannelButton.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <O2/IO/Metadata/UI/ButtonMetadata.hpp>

ChannelButton::ChannelButton() :
    m_button(),
    m_selector()
{
    Initialize();
}

void ChannelButton::Initialize()
{
    m_button = Gx::ResourceManager::Instance()->Create<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_Channel.json");
    SetOrigin(m_button->GetOrigin());
    SetPosition(m_button->GetPosition());

    m_button->SetOrigin(0.f, 0.f);
    m_button->SetPosition(0.f, 0.f);

    m_selector = Gx::ResourceManager::Instance()->Create<Gx::Image>("Metadata/State/Planet/ChannelBoard/ChannelSelector.json");
    AddChild(m_button, m_selector);
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
            metadata = Gx::ResourceManager::Instance()->GetMetadata<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_ChannelHigh.json");
            break;
        case Planet::Kleo:
        case Planet::Philix:
            metadata = Gx::ResourceManager::Instance()->GetMetadata<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_ChannelIntermediate.json");
            break;
        case Planet::Melpomin:
        case Planet::Thalo:
        case Planet::Euta:
            metadata = Gx::ResourceManager::Instance()->GetMetadata<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_ChannelBeginner.json");
            break;
        default:
            metadata = Gx::ResourceManager::Instance()->GetMetadata<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_Channel.json");
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


