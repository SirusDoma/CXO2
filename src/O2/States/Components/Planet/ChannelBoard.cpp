#include <O2/States/Components/Planet/ChannelBoard.hpp>

#include <Genode/Tasks/Action.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/Fx/Move.hpp>

ChannelBoard::ChannelBoard() :
    Gx::Image(),
    Gx::TaskContainer(),
    m_planetInfo(),
    m_tab(),
    m_selectedChannel(),
    m_channelButtons(),
    m_animating(false),
    m_channelPageIndex(),
    m_channelMaxPage(),
    m_noticePageIndex(),
    m_noticeMaxPage()
{
}

void ChannelBoard::Initialize(Gx::Scene &scene)
{
    m_app       = &scene.GetApplication();
    auto& mixer = m_app->Require<Gx::Mixer>();

    m_sfxPopup    = mixer.Create<sf::Sound>("Interface/Metadata/State/Planet/Sound/OpenChannel.json");
    m_sfxNavigate = mixer.Create<sf::Sound>("Interface/Metadata/State/Planet/Sound/ChannelNavigation.json");
    m_sfxEnter    = mixer.Create<sf::Sound>("Interface/Metadata/State/Planet/Sound/ChannelEnter.json");

    m_channelListContainer = std::make_unique<Gx::UiContainer>();
    m_channelListContainer->AddChild(m_channelCategory.get(), m_channelList.get(), m_channelEnterButton.get());

    m_noticePageIndex = 1;
    m_noticeMaxPage   = m_notice->GetFrameCount();
    SwitchTab(Tab::Notice);

    m_renderTexture.create(GetLocalBounds().width, GetLocalBounds().height);
    m_renderTexture.setSmooth(true);
    m_duplicateImage.SetOrigin(GetOrigin());
    m_duplicateImage.SetPosition(GetPosition());
    m_duplicateImage.SetVisible(false);

    AddChild(m_channelTabButton.get(), m_noticeTabButton.get(), m_channelListContainer.get(),
        m_notice.get(), m_currentPageNumber.get(), m_maxPageNumber.get(), m_navigateLeftButton.get(), m_btnNavigateRightButton.get());
}

unsigned int ChannelBoard::GetChannelsPerPage() const
{
    return m_channelsPerPage;
}

void ChannelBoard::SetChannelsPerPage(unsigned int channelsPerPage)
{
    m_channelsPerPage = channelsPerPage;
}

void ChannelBoard::SetNotice(Gx::ResourcePtr<Gx::Image> notice)
{
    m_notice = std::move(notice);
}

void ChannelBoard::SetChannelCategory(Gx::ResourcePtr<Gx::Image> channelCategory)
{
    m_channelCategory = std::move(channelCategory);
}

void ChannelBoard::SetChannelTabButton(Gx::ResourcePtr<Gx::Button> channelTabButton)
{
    m_channelTabButton = std::move(channelTabButton);
    m_channelTabButton->SetClickCallback([=] (auto& sender, auto& ev)
    {
        if (m_tab != Tab::ChannelList && !m_animating && m_planetInfo.Hall != Planet::MusicHall::None)
            SwitchTab(Tab::ChannelList);
    });
}

void ChannelBoard::SetNoticeTabButton(Gx::ResourcePtr<Gx::Button> noticeTabButton)
{
    m_noticeTabButton = std::move(noticeTabButton);
    m_noticeTabButton->SetClickCallback([=] (auto& sender, auto& ev)
    {
        if (m_tab != Tab::Notice && !m_animating)
            SwitchTab(Tab::Notice);
    });
}

void ChannelBoard::SetChannelEnterButton(Gx::ResourcePtr<Gx::Button> btnChannelEnter)
{
    m_channelEnterButton = std::move(btnChannelEnter);
    m_channelEnterButton->SetClickCallback([&] (auto& sender, auto& ev)
    {
        m_app->Require<Gx::Mixer>().Play(m_sfxEnter, "SFX");
        if (m_callback && m_selectedChannel >= 0 && m_selectedChannel < m_planetInfo.Channels.size())
            m_callback(m_planetInfo.Hall, m_planetInfo.Channels[m_selectedChannel]);
    });
}

void ChannelBoard::SetNavigateLeftButton(Gx::ResourcePtr<Gx::Button> btnNavigateLeft)
{
    m_navigateLeftButton = std::move(btnNavigateLeft);
    m_navigateLeftButton->SetClickCallback([&] (auto& sender, auto& ev)
    {
        m_app->Require<Gx::Mixer>().Play(m_sfxNavigate, "SFX");
        if (m_tab == Tab::ChannelList)
        {
            if (m_channelPageIndex > 1)
                ShowChannelList(m_channelPageIndex - 1);
        }
        else
        {
            if (m_noticePageIndex > 1)
                ShowNotice(m_noticePageIndex - 1);
        }
    });
}

void ChannelBoard::SetNavigateRightButton(Gx::ResourcePtr<Gx::Button> btnNavigateRight)
{
    m_btnNavigateRightButton = std::move(btnNavigateRight);
    m_btnNavigateRightButton->SetClickCallback([&] (auto& sender, auto& ev)
    {
        m_app->Require<Gx::Mixer>().Play(m_sfxNavigate, "SFX");
        if (m_tab == Tab::ChannelList)
        {
            if (m_channelPageIndex < m_channelMaxPage)
                ShowChannelList(m_channelPageIndex + 1);
        }
        else
        {
            if (m_noticePageIndex < m_noticeMaxPage)
                ShowNotice(m_noticePageIndex + 1);
        }
    });
}

void ChannelBoard::SetChannelList(Gx::ResourcePtr<Gx::List> list)
{
    m_channelList = std::move(list);
}

void ChannelBoard::SetCurrentPageNumber(Gx::ResourcePtr<Gx::Number> currentPageNumber)
{
    m_currentPageNumber = std::move(currentPageNumber);
    m_currentPageNumber->SetDigitCount(2);
    m_currentPageNumber->SetValue(1);
}

void ChannelBoard::SetMaxPageNumber(Gx::ResourcePtr<Gx::Number> maxPageNumber)
{
    m_maxPageNumber = std::move(maxPageNumber);
    m_maxPageNumber->SetDigitCount(2);
}

void ChannelBoard::AddChannelButton(Gx::ResourcePtr<ChannelButton> button)
{
    button->SetClickCallback([&] (auto& sender, auto& ev) { m_selectedChannel = button->GetChannelNumber() - 1; });
    button->SetDoubleClickCallback([=] (auto& sender, auto& ev) { m_channelEnterButton->PerformClick(); });

    m_channelButtons.push_back(std::move(button));
    m_channelList->AddChild(m_channelButtons.back().get());
}

const sf::FloatRect ChannelBoard::GetLocalBounds() const
{
    return Gx::Image::GetLocalBounds();
}

bool ChannelBoard::InTransition() const
{
    return m_animating;
}

void ChannelBoard::SetEnterChannelCallback(std::function<void(Planet::MusicHall, Planet::ChannelInfo)> callback)
{
    m_callback = callback;
}

void ChannelBoard::CaptureCurrentState()
{
    m_renderTexture.clear(sf::Color::Transparent);
    {
        Render(m_renderTexture, sf::Transform().translate(-GetPosition()));
    }
    m_renderTexture.display();

    m_duplicateImage.SetTexture(m_renderTexture.getTexture());
    m_duplicateImage.SetVisible(true);
}

void ChannelBoard::SwitchTab(ChannelBoard::Tab tab)
{
    m_tab = tab;
    if (m_tab == Tab::ChannelList)
    {
        SetFrame("ChannelList");
        m_notice->SetVisible(false);

        m_channelTabButton->SetVisible(false);
        m_channelTabButton->SetEnabled(false);

        m_noticeTabButton->SetVisible(true);
        m_noticeTabButton->SetEnabled(true);

        m_channelListContainer->SetEnabled(true);
        m_channelListContainer->SetVisible(true);

        m_currentPageNumber->SetValue(m_channelPageIndex);
        m_maxPageNumber->SetValue(m_channelMaxPage);
    }
    else
    {
        SetFrame("Notice");
        m_notice->SetVisible(true);

        m_channelTabButton->SetVisible(true);
        m_channelTabButton->SetEnabled(true);

        m_noticeTabButton->SetVisible(false);
        m_noticeTabButton->SetEnabled(false);

        m_channelListContainer->SetEnabled(false);
        m_channelListContainer->SetVisible(false);

        m_noticePageIndex = 1;
        m_noticeMaxPage   = 1;

        m_currentPageNumber->SetValue(m_noticePageIndex);
        m_maxPageNumber->SetValue(m_noticeMaxPage);
    }
}

void ChannelBoard::Show(Planet::MusicHall hall, std::function<void()> callback)
{
    if (m_animating || m_planetInfo.Hall == hall)
        return;

    m_planetInfo.Hall = hall;
    m_animating = true;

    CaptureCurrentState();
    SwitchTab(Tab::ChannelList);

    auto position = GetPosition();
    SetPosition(800 + GetLocalBounds().width, GetPosition().y);

    m_channelList->SetVisible(false);
    switch (hall)
    {
        case Planet::MusicHall::Kaliope:  m_channelCategory->SetFrame("Kaliope");  break;
        case Planet::MusicHall::Kleo:     m_channelCategory->SetFrame("Kleo");     break;
        case Planet::MusicHall::Philix:   m_channelCategory->SetFrame("Philix");   break;
        case Planet::MusicHall::Melpomin: m_channelCategory->SetFrame("Melpomin"); break;
        case Planet::MusicHall::Thalo:    m_channelCategory->SetFrame("Thalo");    break;
        case Planet::MusicHall::Euta:     m_channelCategory->SetFrame("Euta");     break;
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
        new Gx::Action([=] { m_app->Require<Gx::Mixer>().Play(m_sfxPopup, "SFX"); }),
        new Gx::Move(this, position - sf::Vector2f(30, 0), sf::milliseconds(200)),
        new Gx::Move(this, position, sf::milliseconds(100))
    }));
}

void ChannelBoard::UpdateChannelList(Planet::PlanetInfo info)
{
    m_planetInfo = info;
    m_channelMaxPage = static_cast<int>(std::ceil(static_cast<float>(info.Channels.size()) / CHANNEL_LIST_PER_PAGE));
    m_maxPageNumber->SetValue(m_channelMaxPage);
    m_selectedChannel = 0;

    ShowChannelList(1);
}

void ChannelBoard::ShowChannelList(int page)
{
    if (page <= 0)
        page = 1;

    if (page > m_channelMaxPage)
        page = m_channelMaxPage;

    m_channelPageIndex = page;
    m_currentPageNumber->SetValue(m_channelPageIndex);

    int start = (page - 1) * CHANNEL_LIST_PER_PAGE;
    int end   = start + CHANNEL_LIST_PER_PAGE;
    if (end > m_planetInfo.Channels.size())
        end   = m_planetInfo.Channels.size();

    auto children = m_channelList->GetChildren();
    m_channelList->SetVisible(true);
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

        channelButton->SetPlanet(m_planetInfo.Hall);
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

void ChannelBoard::ShowNotice(int page)
{
    if (page <= 0)
        page = 1;

    if (page > m_noticeMaxPage)
        page = m_noticeMaxPage;

    m_noticePageIndex = page;
    m_currentPageNumber->SetValue(m_noticePageIndex);
    m_notice->SetFrame(m_noticePageIndex - 1);
}

void ChannelBoard::Update(double delta)
{
    Control::Update(delta);
    TaskContainer::Update(delta);
}

sf::RenderStates ChannelBoard::Render(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(m_duplicateImage, states);
    return Image::Render(target, states);
}

void ChannelBoard::Invalidate()
{
}
