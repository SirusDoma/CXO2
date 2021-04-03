#include <O2/Components/Planet/ChannelBoard.hpp>

#include <Genode/Tasks/Action.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Fx/Move.hpp>

#include <O2/Components/Planet/ChannelButton.hpp>

ChannelBoard::ChannelBoard(Gx::Scene &scene) :
    m_planetInfo(),
    m_tab(),
    m_selectedChannel(),
    m_channelButtons(),
    m_animating(false),
    m_scene(&scene)
{
    Initialize();
}

ChannelBoard::~ChannelBoard()
{
}

void ChannelBoard::Initialize()
{
    m_background = m_scene->Create<Gx::Image>("Metadata/State/Planet/ChannelBoard/Background.json");
    m_position   = m_background->GetPosition();

    SetOrigin(m_background->GetOrigin());
    SetPosition(m_position);

    m_background->SetOrigin(0.f, 0.f);
    m_background->SetPosition(0.f, 0.f);

    m_sfxPopup    = m_scene->Create<sf::Sound>("Metadata/State/Planet/Sound/OpenChannel.json", Gx::ResourceScope::Shared);
    m_sfxNavigate = m_scene->Create<sf::Sound>("Metadata/State/Planet/Sound/ChannelNavigation.json", Gx::ResourceScope::Shared);
    m_sfxEnter    = m_scene->Create<sf::Sound>("Metadata/State/Planet/Sound/ChannelEnter.json", Gx::ResourceScope::Shared);

    m_channelListContainer = new Gx::UiContainer();
    m_channelTabButton = m_scene->Create<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_ChannelTab.json");
    m_channelTabButton->SetClickCallback([=] (auto& sender, auto& ev) {
        if (m_tab != Tab::ChannelList && !m_animating && m_planetInfo.Planet)
            SwitchTab(Tab::ChannelList);
    });

    m_channelCategory = m_scene->Create<Gx::Image>("Metadata/State/Planet/ChannelBoard/ChannelCategory.json");
    m_channelListContainer->AddChild(m_channelCategory);

    m_repeater = m_scene->Create<Gx::Repeater>("Metadata/State/Planet/ChannelBoard/ChannelList.json");
    auto base = m_scene->GetLocalResources().Resolve<Gx::RadioButton>("Metadata/State/Planet/ChannelBoard/Btn_Channel/Background.json");
    for (int i = 0; i < CHANNEL_LIST_PER_PAGE; i++)
    {
        auto channelButton = std::make_unique<ChannelButton>(*m_scene, *base);
        channelButton->SetChannelNumber(i + 1);
        channelButton->SetClickCallback([=] (auto& sender, auto& ev) {
            m_selectedChannel = i;
        });

        m_repeater->AddChild(channelButton.get());
        m_channelButtons.push_back(std::move(channelButton));
    }
    m_channelListContainer->AddChild(m_repeater);

    m_currentPageNumber = m_scene->Create<Gx::Number>("Metadata/State/Planet/ChannelBoard/ChannelCurrentPageNumber.json");
    m_maxPageNumber     = m_scene->Create<Gx::Number>("Metadata/State/Planet/ChannelBoard/ChannelMaxPageNumber.json");
    m_currentPageNumber->SetDigitCount(2);
    m_maxPageNumber->SetDigitCount(2);
    m_channelListContainer->AddChild(m_currentPageNumber, m_maxPageNumber);

    auto btnChannelEnter = m_scene->Create<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_ChannelEnter.json");
    btnChannelEnter->SetClickCallback([=] (auto& sender, auto& ev)
    {
        m_sfxEnter->play();
        if (m_callback && m_selectedChannel >= 0 && m_selectedChannel < m_planetInfo.Channels.size())
            m_callback(m_planetInfo.Planet, m_planetInfo.Channels[m_selectedChannel]);
    });

    auto btnChannelLeft  = m_scene->Create<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_ChannelLeft.json");
    btnChannelLeft->SetClickCallback([=] (auto& sender, auto& ev)
    {
        m_sfxNavigate->play();
        if (m_currentPageNumber->GetValue() > 1)
            ShowPage(m_currentPageNumber->GetValue() - 1);
    });

    auto btnChannelRight = m_scene->Create<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_ChannelRight.json");
    btnChannelRight->SetClickCallback([=] (auto& sender, auto& ev)
    {
        m_sfxNavigate->play();
        if (m_currentPageNumber->GetValue() < m_maxPageNumber->GetValue())
            ShowPage(m_currentPageNumber->GetValue() + 1);
    });

    m_channelListContainer->AddChild(btnChannelEnter, btnChannelLeft, btnChannelRight);

    m_notice = m_scene->Create<Gx::Image>("Metadata/State/Planet/ChannelBoard/Notice.json");
    m_noticeTabButton  = m_scene->Create<Gx::Button>("Metadata/State/Planet/ChannelBoard/Btn_NoticeTab.json");
    m_noticeTabButton->SetClickCallback([=] (auto& sender, auto& ev)
    {
        if (m_tab != Tab::Notice && !m_animating)
            SwitchTab(Tab::Notice);
    });

    SwitchTab(Tab::Notice);
    m_duplicateTexture.create(GetLocalBounds().width, GetLocalBounds().height);
    m_duplicateTexture.setSmooth(true);

    m_duplicateImage.SetOrigin(GetOrigin());
    m_duplicateImage.SetPosition(m_position);
    m_duplicateImage.SetVisible(false);

    AddChild(m_background, m_channelTabButton, m_noticeTabButton, m_channelListContainer, m_notice);
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

void ChannelBoard::SetEnterChannelCallback(std::function<void(Planet, ChannelInfo)> callback)
{
    m_callback = callback;
}

void ChannelBoard::CaptureCurrentState()
{
    m_duplicateTexture.clear(sf::Color::Transparent);
    {
        Render(m_duplicateTexture, sf::Transform().translate(-GetPosition()));
    }
    m_duplicateTexture.display();
    m_duplicateImage.SetTexture(m_duplicateTexture.getTexture());
    m_duplicateImage.SetVisible(true);
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

        m_channelListContainer->SetEnabled(true);
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

        m_channelListContainer->SetEnabled(false);
        m_channelListContainer->SetVisible(false);
    }
}

void ChannelBoard::Show(Planet planet, std::function<void()> callback)
{
    if (m_animating || m_planetInfo.Planet == planet)
        return;

    m_planetInfo.Planet = planet;
    m_animating = true;

    CaptureCurrentState();
    SwitchTab(Tab::ChannelList);
    SetPosition(800 + m_background->GetLocalBounds().width, m_position.y);

    m_repeater->SetVisible(false);
    switch (planet)
    {
        case Planet::Kaliope:  m_channelCategory->SetFrame("Kaliope");  break;
        case Planet::Kleo:     m_channelCategory->SetFrame("Kleo");     break;
        case Planet::Philix:   m_channelCategory->SetFrame("Philix");   break;
        case Planet::Melpomin: m_channelCategory->SetFrame("Melpomin"); break;
        case Planet::Thalo:    m_channelCategory->SetFrame("Thalo");    break;
        case Planet::Euta:     m_channelCategory->SetFrame("Euta");     break;
        default: break;
    }

    Run(new Gx::Sequence([=]
    {
        m_animating = false;
        m_duplicateImage.SetVisible(false);
        if (callback)
            callback();
    },
    {
        new Gx::Action([=] { m_scene->Play(m_sfxPopup); }),
        new Gx::Move(this, m_position - sf::Vector2f(30, 0), sf::milliseconds(200)),
        new Gx::Move(this, m_position, sf::milliseconds(100))
    }));
}

void ChannelBoard::UpdateChannelList(PlanetInfo info)
{
    m_planetInfo = info;
    m_maxPageNumber->SetValue(static_cast<int>(std::ceil(static_cast<float>(info.Channels.size()) / CHANNEL_LIST_PER_PAGE)));
    m_selectedChannel = 0;

    ShowPage(1);
}

void ChannelBoard::ShowPage(int page)
{
    if (page <= 0)
        page = 1;

    if (page > m_maxPageNumber->GetValue())
        page = m_maxPageNumber->GetValue();

    m_currentPageNumber->SetValue(page);
    int start = (page - 1) * CHANNEL_LIST_PER_PAGE;
    int end   = start + CHANNEL_LIST_PER_PAGE;
    if (end > m_planetInfo.Channels.size())
        end   = m_planetInfo.Channels.size();

    auto children = m_repeater->GetChildren();
    m_repeater->SetVisible(true);
    for (int i = 0; i < CHANNEL_LIST_PER_PAGE; i++)
    {
        auto channelButton = dynamic_cast<ChannelButton*>(children[i]);
        if (!channelButton)
            continue;

        int channelIndex = start + i;
        if (channelIndex >= end)
        {
            channelButton->SetVisible(false);
            channelButton->SetEnabled(false);

            continue;
        }

        channelButton->SetPlanet(m_planetInfo.Planet);
        channelButton->SetChannelNumber(channelIndex + 1);
        channelButton->SetCheckedState(channelIndex == m_selectedChannel);
        channelButton->SetChannelPopulation(m_planetInfo.Channels[channelIndex].Population);
        channelButton->SetVisible(true);
        channelButton->SetEnabled(true);
        channelButton->SetClickCallback([=] (auto& sender, auto& ev)
        {
            m_selectedChannel = channelIndex;
        });
    }
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
