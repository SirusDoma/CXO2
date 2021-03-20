#include <O2/Components/Planet/ChannelBoard.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Tasks/Action.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Fx.hpp>

#include <O2/Components/Planet/ChannelButton.hpp>

ChannelBoard::ChannelBoard() :
    m_position(),
    m_background(),
    m_channelTabButton(),
    m_noticeTabButton(),
    m_notice(),
    m_channelButton(),
    m_channelListContainer(),
    m_duplicateImage(),
    m_duplicateTexture(),
    m_sfx(),
    m_sfxBuffer(),
    m_planet(),
    m_tab(),
    m_animating(false)
{
    Initialize();
}

void ChannelBoard::Initialize()
{
    m_background = Gx::ResourceManager::Instance()->Create<Gx::Image>("Metadata/State/Planet/ChannelBoard/Background.json");
    m_position   = m_background->GetPosition();

    SetOrigin(m_background->GetOrigin());
    SetPosition(m_position);

    m_background->SetOrigin(0.f, 0.f);
    m_background->SetPosition(0.f, 0.f);

    m_channelListContainer = new Gx::UiContainer();
    m_channelTabButton = Gx::ResourceManager::Instance()->Create<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_ChannelTab.json");
    m_channelTabButton->SetClickCallback([this] {
        if (m_tab != Tab::ChannelList && !m_animating && m_planet)
            SwitchTab(Tab::ChannelList);
    });

    m_channelButton = new ChannelButton();
    m_channelListContainer->AddChild(m_channelButton);

    auto btnChannelEnter = Gx::ResourceManager::Instance()->Create<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_ChannelEnter.json");
    auto btnChannelLeft  = Gx::ResourceManager::Instance()->Create<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_ChannelLeft.json");
    auto btnChannelRight = Gx::ResourceManager::Instance()->Create<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_ChannelRight.json");
    m_channelListContainer->AddChild(btnChannelEnter, btnChannelLeft, btnChannelRight);

    m_notice = Gx::ResourceManager::Instance()->Create<Gx::Image>("Metadata/State/Planet/ChannelBoard/Notice.json");
    m_noticeTabButton  = Gx::ResourceManager::Instance()->Create<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_NoticeTab.json");
    m_noticeTabButton->SetClickCallback([this] {
        if (m_tab != Tab::Notice && !m_animating)
            SwitchTab(Tab::Notice);
    });

    Gx::Uint8 *data;
    auto size = Gx::ResourceManager::Instance()->GetResourceData("Planet/openChannel", &data);

    m_sfx = sf::Sound();
    m_sfxBuffer = sf::SoundBuffer();
    if (m_sfxBuffer.loadFromMemory(data, size))
        m_sfx.setBuffer(m_sfxBuffer);

    SwitchTab(Tab::Notice);
    m_duplicateTexture.create(GetLocalBounds().width, GetLocalBounds().height);
    m_duplicateTexture.setSmooth(true);

    m_duplicateImage.SetOrigin(GetOrigin());
    m_duplicateImage.SetPosition(m_position);
    m_duplicateImage.SetVisible(false);

    AddChild(m_background,  m_channelTabButton, m_noticeTabButton, m_channelListContainer, m_notice);
}

ChannelBoard::~ChannelBoard()
{
}

const sf::FloatRect ChannelBoard::GetLocalBounds() const
{
    if (m_background)
        return m_background->GetLocalBounds();

    return sf::FloatRect();
}

bool ChannelBoard::InTransition() const
{
    return m_animating;
}

void ChannelBoard::SwitchTab(ChannelBoard::Tab tab)
{
    if (!m_background)
        return;

    m_tab = tab;
    if (m_tab == Tab::ChannelList)
    {
        m_background->SetFrame("ChannelList");
        m_notice->SetVisible(false);

        m_channelTabButton->SetVisible(false);
        m_channelTabButton->SetEnabled(false);

        m_noticeTabButton->SetVisible(true);
        m_noticeTabButton->SetEnabled(true);

        m_channelListContainer->SetVisible(true);
    }
    else
    {
        m_background->SetFrame("Notice");
        m_notice->SetVisible(true);

        m_channelTabButton->SetVisible(true);
        m_channelTabButton->SetEnabled(true);

        m_noticeTabButton->SetVisible(false);
        m_noticeTabButton->SetEnabled(false);

        m_channelListContainer->SetVisible(false);
    }
}

void ChannelBoard::Show(Planet planet, std::function<void()> callback)
{
    if (m_animating)
        return;

    m_planet    = planet;
    m_animating = true;

    m_duplicateTexture.clear(sf::Color::Transparent);
    {
        Render(m_duplicateTexture, sf::Transform().translate(-GetPosition()));
    }
    m_duplicateTexture.display();
    m_duplicateImage.SetTexture(m_duplicateTexture.getTexture());
    m_duplicateImage.SetVisible(true);

    m_channelButton->SetPlanet(planet);
    SwitchTab(Tab::ChannelList);
    SetPosition(800 + m_background->GetLocalBounds().width, m_position.y);

    Run(new Gx::Sequence([=] {
        m_animating = false;
        m_duplicateImage.SetVisible(false);
        if (callback)
            callback();
    }, {
        new Gx::Action([=] { m_sfx.play(); }),
        new Gx::Move(this, m_position - sf::Vector2f(30, 0), sf::milliseconds(200)),
        new Gx::Move(this, m_position, sf::milliseconds(100))
    }));
}

void ChannelBoard::Update(double delta)
{
    Control::Update(delta);
    TaskContainer::Update(delta);
}

sf::RenderStates ChannelBoard::Render(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(m_duplicateImage, states);
    return Control::Render(target, states);
}

void ChannelBoard::Invalidate()
{
}
