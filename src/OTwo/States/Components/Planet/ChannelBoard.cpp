#include <OTwo/States/Components/Planet/ChannelBoard.hpp>
#include <OTwo/States/State.hpp>
#include <OTwo/States/Components/Planet/ChannelButton.hpp>

#include <Genode/Tasks.hpp>
#include <Genode/Fx.hpp>

#include <cmath>

ChannelBoard::ChannelBoard() :
    m_tab(ChannelBoard::Tab::Notice),
    m_transitioning(false),
    m_animationEnabled(true),
    m_channelsPerPage(),
    m_channelPageIndex(),
    m_channelMaxPage(),
    m_noticePageIndex(),
    m_noticeMaxPage(),
    m_channelButton(),
    m_selectedChannel()
{
}

void ChannelBoard::Initialize()
{
    const auto parent = GetParent<::State>();
    if (!parent)
        return;

    auto& app            = Gx::Application::Instance();
    auto& mixer          = app.Require<Gx::Mixer>();
    auto sfxNavigate     = parent->Load<sf::Sound>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_SOUND_CHANNEL_PAGE");
    auto sfxEnter        = parent->Load<sf::Sound>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_SOUND_CHANNEL_ENTER");
    const auto container = parent->Create<Gx::UiContainer>();
    container->SetName("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_CONTAINER_CONTROLS");
    AddChild(container);

    const auto category  = parent->Load<Gx::Image>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_IMAGE_CHANNEL_CATEGORY");
    container->AddChild(category);

    const auto channelTabButton = parent->Load<Gx::Button>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_BUTTON_CHANNEL_TAB");
    channelTabButton->SetEnabled(false);
    channelTabButton->SetClickCallback(
        [=] (auto& sender, auto& ev)
        {
            if (m_tab == Tab::ChannelList || m_transitioning || m_planetInfo.Hall == MusicHall::None)
            {
                ev.Handled = true;
                return;
            }

            SwitchTab(Tab::ChannelList);
        }
    );

    const auto noticeTabButton = parent->Load<Gx::Button>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_BUTTON_NOTICE_TAB");
    noticeTabButton->SetClickCallback(
        [=] (auto& sender, auto& ev)
        {
            if (m_tab == Tab::Notice || m_transitioning)
            {
                ev.Handled = true;
                return;
            }

            SwitchTab(Tab::Notice);
        }
    );

    const auto channelList = parent->Load<Gx::List>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_LIST_CHANNEL");
    container->AddChild(channelList);

    const auto currentPageNumber = parent->Load<Gx::Number>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_NUMBER_CURRENT_CHANNEL_PAGE");
    const auto maxPageNumber     = parent->Load<Gx::Number>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_NUMBER_MAX_CHANNEL_PAGE");
    currentPageNumber->SetDigitCount(2);
    maxPageNumber->SetDigitCount(2);

    const auto enterButton = parent->Load<Gx::Button>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_BUTTON_CHANNEL_ENTER");
    container->AddChild(enterButton);
    enterButton->SetClickCallback(
        [&, sfxEnter] (auto& sender, auto& ev)
        {
            mixer.Play(sfxEnter, "SFX");
            if (m_callback && m_selectedChannel >= 0 && m_selectedChannel < m_planetInfo.Channels.size())
                m_callback(m_planetInfo.Hall, m_planetInfo.Channels[m_selectedChannel]);
        }
    );

    const auto channelButtons = channelList->GetChildren();
    for (int i = 0; i < channelButtons.size(); i++)
    {
        auto channelButton = dynamic_cast<ChannelButton*>(channelButtons[i]);
        channelButton->SetChannelNumber(i + 1);
        channelButton->SetClickCallback(
            [&] (auto& sender, auto& ev)
            {
                m_selectedChannel = channelButton->GetChannelNumber() - 1;
            }
        );
        channelButton->SetDoubleClickCallback(
            [=] (auto& sender, auto& ev)
            {
                enterButton->PerformClick();
            }
        );

        channelButton->Initialize();
    }

    const auto leftButton = parent->Load<Gx::Button>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_BUTTON_CHANNEL_LEFT");
    leftButton->SetClickCallback(
        [&, sfxNavigate] (auto& sender, auto& ev)
        {
            mixer.Play(sfxNavigate, "SFX");
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
        }
    );

    const auto rightButton = parent->Load<Gx::Button>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_BUTTON_CHANNEL_RIGHT");
    rightButton->SetClickCallback(
        [&, sfxNavigate] (auto& sender, auto& ev)
        {
            mixer.Play(sfxNavigate, "SFX");
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
        }
    );

    const auto notice = parent->Load<Gx::Image>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_IMAGE_CHANNEL_NOTICE");
    m_noticePageIndex = 1;
    m_noticeMaxPage   = notice->GetFrameCount();

    if (const auto bounds = GetLocalBounds(); !m_renderTexture.create(sf::Vector2u(bounds.width, bounds.height)))
        throw Gx::Exception("Failed to create render texture.");

    m_renderTexture.setSmooth(true);
    m_duplicateImage.SetOrigin(GetOrigin());
    m_duplicateImage.SetPosition(GetPosition());
    m_duplicateImage.SetVisible(false);

    AddChild(notice, channelTabButton, noticeTabButton, currentPageNumber, maxPageNumber, leftButton, rightButton);
    SwitchTab(Tab::Notice);
}


void ChannelBoard::SetChannelButton(ChannelButton &button)
{
    m_channelButton = &button;
}

unsigned int ChannelBoard::GetChannelsPerPage() const
{
    return m_channelsPerPage;
}

void ChannelBoard::SetChannelsPerPage(const unsigned int channelsPerPage)
{
    m_channelsPerPage = channelsPerPage;
}

sf::FloatRect ChannelBoard::GetLocalBounds() const
{
    return Gx::Image::GetLocalBounds();
}

bool ChannelBoard::InTransition() const
{
    return m_transitioning;
}

bool ChannelBoard::IsAnimationEnabled() const
{
    return m_animationEnabled;
}

void ChannelBoard::SetAnimationEnabled(const bool animationEnabled)
{
    m_animationEnabled = animationEnabled;
}

void ChannelBoard::SetChannelEnterCallback(std::function<void(MusicHall, ServerChannel)> callback)
{
    m_callback = std::move(callback);
}

void ChannelBoard::CaptureCurrentState()
{
    m_renderTexture.clear(sf::Color::Transparent);
    {
        auto adapter = Gx::RenderTargetAdapter(m_renderTexture);
        Render(adapter, sf::Transform().translate(-GetPosition()));
    }
    m_renderTexture.display();

    m_duplicateImage.SetTexture(m_renderTexture.getTexture());
    m_duplicateImage.SetVisible(true);
}

void ChannelBoard::SwitchTab(const ChannelBoard::Tab tab)
{
    m_tab = tab;

    const auto notice            = FindChild<Gx::Image>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_IMAGE_CHANNEL_NOTICE");
    const auto channelTabButton  = FindChild<Gx::Button>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_BUTTON_CHANNEL_TAB");
    const auto noticeTabButton   = FindChild<Gx::Button>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_BUTTON_NOTICE_TAB");
    const auto container         = FindChild<Gx::UiContainer>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_CONTAINER_CONTROLS");
    const auto currentPageNumber = FindChild<Gx::Number>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_NUMBER_CURRENT_CHANNEL_PAGE");
    const auto maxPageNumber     = FindChild<Gx::Number>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_NUMBER_MAX_CHANNEL_PAGE");

    if (m_tab == Tab::ChannelList)
    {
        SetFrame("ChannelList");
        notice->SetVisible(false);

        channelTabButton->SetVisible(false);
        channelTabButton->SetEnabled(false);

        noticeTabButton->SetVisible(true);
        noticeTabButton->SetEnabled(true);

        container->SetEnabled(true);
        container->SetVisible(true);

        currentPageNumber->SetValue(m_channelPageIndex);
        maxPageNumber->SetValue(m_channelMaxPage);
    }
    else
    {
        SetFrame("Notice");
        notice->SetVisible(true);

        if (m_planetInfo.Hall != MusicHall::None)
        {
            channelTabButton->SetVisible(true);
            channelTabButton->SetEnabled(true);
        }

        noticeTabButton->SetVisible(false);
        noticeTabButton->SetEnabled(false);

        container->SetEnabled(false);
        container->SetVisible(false);

        m_noticePageIndex = 1;
        m_noticeMaxPage   = 1;

        currentPageNumber->SetValue(m_noticePageIndex);
        maxPageNumber->SetValue(m_noticeMaxPage);
    }
}

void ChannelBoard::Show(const MusicHall hall, std::function<void()> callback)
{
    if (m_transitioning || m_planetInfo.Hall == hall)
        return;

    const auto parent = GetParent<::State>();
    const auto sfxPopup    = parent->Load<sf::Sound>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_SOUND_CHANNEL_OPEN");
    const auto container   = FindChild<Gx::UiContainer>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_CONTAINER_CONTROLS");
    const auto category    = container->FindChild<Gx::Image>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_IMAGE_CHANNEL_CATEGORY");
    const auto channelList = container->FindChild<Gx::List>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_LIST_CHANNEL");

    if (m_animationEnabled)
        CaptureCurrentState();

    m_planetInfo.Hall = hall;
    switch (hall)
    {
        case MusicHall::Kalliope: category->SetFrame("Kalliope");  break;
        case MusicHall::Kleo:     category->SetFrame("Kleo");     break;
        case MusicHall::Philix:   category->SetFrame("Philix");   break;
        case MusicHall::Melpomin: category->SetFrame("Melpomin"); break;
        case MusicHall::Thalo:    category->SetFrame("Thalo");    break;
        case MusicHall::Euta:     category->SetFrame("Euta");     break;
        default: break;
    }

    SwitchTab(Tab::ChannelList);
    channelList->SetVisible(false);

    if (m_animationEnabled)
    {
        m_transitioning = true;
        auto position = GetPosition();
        SetPosition(800 + GetLocalBounds().width, GetPosition().y);

        const auto sequence = parent->Create<Gx::Sequence>([&, callback]
        {
            m_transitioning = false;
            m_duplicateImage.SetVisible(false);
            if (callback)
                callback();
        },
        Gx::Sequence::ListOf({
            //parent->Create<Gx::Action>([&, sfxPopup] { mixer.Play(sfxPopup, "SFX"); }),
            parent->Create<Gx::Move>(this, position - sf::Vector2f(30, 0), sf::milliseconds(200)),
            parent->Create<Gx::Move>(this, position, sf::milliseconds(200))
        }));

        Run(sequence);
    }
    else
    {
        m_transitioning = false;
        if (callback)
            callback();
    }
}

void ChannelBoard::UpdateChannelList(const PlanetInfo &planet)
{
    m_planetInfo = planet;
    m_channelMaxPage = static_cast<int>(std::ceil(static_cast<float>(planet.Channels.size()) / CHANNEL_LIST_PER_PAGE));

    const auto maxPageNumber = FindChild<Gx::Number>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_NUMBER_MAX_CHANNEL_PAGE");
    maxPageNumber->SetValue(m_channelMaxPage);

    m_selectedChannel = 0;
    ShowChannelList(1);
}

void ChannelBoard::ShowChannelList(unsigned int page)
{
    if (page <= 0)
        page = 1;

    if (page > m_channelMaxPage)
        page = m_channelMaxPage;

    const auto currentPageNumber = FindChild<Gx::Number>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_NUMBER_CURRENT_CHANNEL_PAGE");
    const auto container         = FindChild<Gx::UiContainer>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_CONTAINER_CONTROLS");
    const auto channelList       = container->FindChild<Gx::List>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_LIST_CHANNEL");

    m_channelPageIndex = page;
    currentPageNumber->SetValue(m_channelPageIndex);

    const int start = (page - 1) * m_channelsPerPage;
    int end = start + m_channelsPerPage;
    if (end > m_planetInfo.Channels.size())
        end = m_planetInfo.Channels.size();

    const auto children = channelList->GetChildren();
    channelList->SetVisible(true);
    for (int i = 0; i < m_channelsPerPage; i++)
    {
        const auto channelButton = dynamic_cast<ChannelButton*>(children[i]);
        if (!channelButton)
            continue;

        const int channelIndex = start + i;
        if (channelIndex >= end)
        {
            channelButton->SetVisible(false);
            channelButton->SetEnabled(false);

            continue;
        }

        channelButton->SetChannelNumber(channelIndex + 1);
        channelButton->SetMusicHall(m_planetInfo.Hall);
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

void ChannelBoard::ShowNotice(unsigned int page)
{
    if (page <= 0)
        page = 1;

    if (page > m_noticeMaxPage)
        page = m_noticeMaxPage;

    auto currentPageNumber = FindChild<Gx::Number>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_NUMBER_CURRENT_CHANNEL_PAGE");
    auto notice            = FindChild<Gx::Image>("STATE_PLANET/IDC_CHANNEL_BOARD/IDC_IMAGE_CHANNEL_NOTICE");

    m_noticePageIndex = page;
    currentPageNumber->SetValue(m_noticePageIndex);
    notice->SetFrame(m_noticePageIndex - 1);
}

void ChannelBoard::Update(const double delta)
{
    Control::Update(delta);
    TaskContainer::Update(delta);
}

Gx::RenderStates ChannelBoard::Render(Gx::RenderSurface &surface, Gx::RenderStates states) const
{
    if (m_animationEnabled)
        surface.Render(m_duplicateImage, states);

    return Image::Render(surface, states);
}

void ChannelBoard::Invalidate()
{
    Gx::Image::Invalidate();
}
