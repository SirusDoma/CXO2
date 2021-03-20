#include <O2/Components/Planet/ChannelBoard.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Tasks/Action.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Fx.hpp>

ChannelBoard::ChannelBoard() :
    m_position(),
    m_texCoords(),
    m_background(),
    m_notice(),
    m_sfx(),
    m_sfxBuffer(),
    m_planet(),
    m_tab(Tab::Notice),
    m_animating(false)
{
    Initialize();
}

void ChannelBoard::Initialize()
{
    m_background = Gx::ResourceManager::Instance()->Create<Gx::Sprite>("Metadata/State/Planet/ChannelBoard/Background.json");
    m_texCoords = m_background->GetTexCoords();
    m_position  = m_background->GetPosition();

    SetOrigin(m_background->GetOrigin());
    SetPosition(m_position);

    m_background->SetOrigin(0.f, 0.f);
    m_background->SetPosition(0.f, 0.f);

    m_notice = Gx::ResourceManager::Instance()->Create<Gx::Sprite>("Metadata/State/Planet/ChannelBoard/Notice.json");

    Gx::Uint8 *data;
    auto size = Gx::ResourceManager::Instance()->GetResourceData("openChannel", &data);

    m_sfx = sf::Sound();
    m_sfxBuffer = sf::SoundBuffer();
    if (m_sfxBuffer.loadFromMemory(data, size))
        m_sfx.setBuffer(m_sfxBuffer);

    SwitchTab(m_tab);
}

ChannelBoard::~ChannelBoard()
{
}

bool ChannelBoard::InTransition() const
{
    return m_animating;
}

void ChannelBoard::SwitchTab(ChannelBoard::Tab tab)
{
    if (!m_background)
        return;

    // TODO: Set Button tab active
    m_tab = tab;
    if (m_tab == Tab::ChannelList)
        m_background->SetTexCoords(sf::IntRect(0, 0, m_texCoords.width, m_texCoords.height));
    else
        m_background->SetTexCoords(sf::IntRect(m_texCoords.width, 0, m_texCoords.width, m_texCoords.height));
}

void ChannelBoard::Show(Planet planet, std::function<void()> callback)
{
    if (m_animating)
        return;

    m_planet    = planet;
    m_animating = true;

    SwitchTab(Tab::ChannelList);
    SetPosition(800 + m_background->GetLocalBounds().width, m_position.y);

    Run(new Gx::Sequence([=] {
        m_animating = false;
        if (callback)
            callback();
    }, {
        new Gx::Action([=] { m_sfx.play(); }),
        new Gx::Move(this, m_position - sf::Vector2f(30, 0), sf::milliseconds(200)),
        new Gx::Move(this, m_position, sf::milliseconds(100))
    }));
}

sf::RenderStates ChannelBoard::Render(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= GetTransform();
    target.draw(*m_background, states);

    if (m_tab == Tab::Notice)
        target.draw(*m_notice, states);

    return states;
}
