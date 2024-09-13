#include <OTwo/UI/Planet/ChannelBoard.hpp>
#include <OTwo/States/State.hpp>
#include <OTwo/UI/Planet/ChannelButton.hpp>

#include <Genode/Tasks.hpp>
#include <Genode/Fx.hpp>

#include <cmath>

ChannelBoard::ChannelBoard(Gx::Mixer& mixer, Gx::ResourceManager& resources) :
    m_mixer(mixer),
    m_resources(resources),
    m_channelButton(),
    m_captureImage(),
    m_tab(ChannelBoard::Tab::Notice),
    m_sequence(),
    m_moveIn(this, {}, sf::Time::Zero),
    m_moveOut(this, {}, sf::Time::Zero),
    m_transitioning(false),
    m_animationEnabled(true),
    m_selectedChannel(),
    m_channelsPerPage(),
    m_channelPageIndex(),
    m_channelMaxPage(),
    m_noticePageIndex(),
    m_noticeMaxPage()
{
}

void ChannelBoard::Initialize()
{
    Image::Initialize();

    auto sfxNavigate     = &m_resources.AddFromFile<sf::Sound>("bgEffect/07");
    auto sfxEnter        = &m_resources.AddFromFile<sf::Sound>("bgEffect/10");
    const auto container = FindChild<Gx::UiContainer>("IDC_CONTAINER_CHANNEL_CONTROLS");

    const auto channelTabButton = FindChild<Gx::Button>("IDC_BUTTON_CHANNEL_TAB");
    channelTabButton->SetEnabled(false);
    channelTabButton->SetClickCallback(
        [=] (auto&, auto& ev)
        {
            if (m_tab == Tab::ChannelList || m_transitioning || m_planetInfo.Hall == MusicHall::None)
            {
                ev.Handled = true;
                return;
            }

            SwitchTab(Tab::ChannelList);
        }
    );

    const auto noticeTabButton = FindChild<Gx::Button>("IDC_BUTTON_NOTICE_TAB");
    noticeTabButton->SetClickCallback(
        [=] (auto&, auto& ev)
        {
            if (m_tab == Tab::Notice || m_transitioning)
            {
                ev.Handled = true;
                return;
            }

            SwitchTab(Tab::Notice);
        }
    );

    const auto channelList       = container->FindChild<Gx::List>("IDC_LIST_CHANNEL");
    const auto currentPageNumber = FindChild<Gx::BitmapNumber>("IDC_NUMBER_CURRENT_CHANNEL_PAGE");
    const auto maxPageNumber     = FindChild<Gx::BitmapNumber>("IDC_NUMBER_MAX_CHANNEL_PAGE");
    currentPageNumber->SetDigitCount(2);
    maxPageNumber->SetDigitCount(2);

    const auto enterButton = container->FindChild<Gx::Button>("IDC_BUTTON_CHANNEL_ENTER");
    enterButton->SetClickCallback(
        [this, sfxEnter] (auto&, auto&)
        {
            m_mixer.Play(sfxEnter, "SFX");
            if (m_callback && m_selectedChannel >= 0 && m_selectedChannel < m_planetInfo.Channels.size())
                m_callback(m_planetInfo.Hall, m_planetInfo.Channels[m_selectedChannel]);
        }
    );

    const auto channelButtons = channelList->GetChildren();
    for (std::size_t i = 0; i < channelButtons.size(); i++)
    {
        auto channelButton = dynamic_cast<ChannelButton*>(channelButtons[i]);
        channelButton->SetChannelNumber(i + 1);
        channelButton->SetClickCallback(
            [&] (auto&, auto&)
            {
                m_selectedChannel = channelButton->GetChannelNumber() - 1;
            }
        );
        channelButton->SetDoubleClickCallback(
            [=] (auto&, auto&)
            {
                enterButton->PerformClick();
            }
        );
    }

    const auto leftButton = FindChild<Gx::Button>("IDC_BUTTON_CHANNEL_LEFT");
    leftButton->SetClickCallback(
        [this, sfxNavigate] (auto&, auto&)
        {
            m_mixer.Play(sfxNavigate, "SFX");
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

    const auto rightButton = FindChild<Gx::Button>("IDC_BUTTON_CHANNEL_RIGHT");
    rightButton->SetClickCallback(
        [this, sfxNavigate] (auto&, auto&)
        {
            m_mixer.Play(sfxNavigate, "SFX");
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

    const auto notice = FindChild<Gx::Image>("IDC_IMAGE_CHANNEL_NOTICE");
    m_noticePageIndex = 1;
    m_noticeMaxPage   = notice->GetFrameCount();

    if (const auto bounds = GetLocalBounds(); !m_renderTexture.resize(sf::Vector2u(bounds.size.x, bounds.size.y)))
        throw Gx::Exception("Failed to create render texture");

    m_renderTexture.setSmooth(true);
    m_captureImage.SetOrigin(GetOrigin());
    m_captureImage.SetPosition(GetPosition());
    m_captureImage.SetVisible(false);

    AddChild(notice, channelTabButton, noticeTabButton, currentPageNumber, maxPageNumber, leftButton, rightButton);
    SwitchTab(Tab::Notice);
}


void ChannelBoard::SetChannelButton(ChannelButton& button)
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

    m_captureImage.SetTexture(m_renderTexture.getTexture());
    m_captureImage.SetVisible(true);
}

void ChannelBoard::SwitchTab(const ChannelBoard::Tab tab)
{
    m_tab = tab;

    const auto notice            = FindChild<Gx::Image>("IDC_IMAGE_CHANNEL_NOTICE");
    const auto channelTabButton  = FindChild<Gx::Button>("IDC_BUTTON_CHANNEL_TAB");
    const auto noticeTabButton   = FindChild<Gx::Button>("IDC_BUTTON_NOTICE_TAB");
    const auto container         = FindChild<Gx::UiContainer>("IDC_CONTAINER_CHANNEL_CONTROLS");
    const auto currentPageNumber = FindChild<Gx::BitmapNumber>("IDC_NUMBER_CURRENT_CHANNEL_PAGE");
    const auto maxPageNumber     = FindChild<Gx::BitmapNumber>("IDC_NUMBER_MAX_CHANNEL_PAGE");

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

    const auto sfxPopup    = &m_resources.AddFromFile<sf::Sound>("Planet/openChannel");
    const auto container   = FindChild<Gx::UiContainer>("IDC_CONTAINER_CHANNEL_CONTROLS");
    const auto category    = container->FindChild<Gx::Image>("IDC_IMAGE_CHANNEL_CATEGORY");
    const auto channelList = container->FindChild<Gx::List>("IDC_LIST_CHANNEL");

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
        const auto position = GetPosition();
        SetPosition(800 + GetLocalBounds().size.x, position.y);
        Stop(&m_sequence);

        m_moveIn  = Gx::Move(this, position - sf::Vector2f(30, 0), sf::milliseconds(200));
        m_moveOut = Gx::Move(this, position, sf::milliseconds(200));

        m_sequence = Gx::Sequence([&, callback]
        {
            m_transitioning = false;
            m_captureImage.SetVisible(false);
            if (callback)
                callback();
        },
        Gx::Sequence::ListOf(
        {
            //parent->Create<Gx::Action>([&, sfxPopup] { mixer.Play(sfxPopup, "SFX"); }),
            &m_moveIn,
            &m_moveOut
        }));

        Run(&m_sequence);
    }
    else
    {
        m_transitioning = false;
        if (callback)
            callback();
    }
}

void ChannelBoard::UpdateChannelList(const PlanetInfo& planet)
{
    const auto container   = FindChild<Gx::UiContainer>("IDC_CONTAINER_CHANNEL_CONTROLS");
    const auto channelList = container->FindChild<Gx::List>("IDC_LIST_CHANNEL");

    m_planetInfo = planet;
    m_channelMaxPage = static_cast<int>(std::ceil(static_cast<float>(planet.Channels.size()) / channelList->GetChildren().size()));

    const auto maxPageNumber = FindChild<Gx::BitmapNumber>("IDC_NUMBER_MAX_CHANNEL_PAGE");
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

    const auto currentPageNumber = FindChild<Gx::BitmapNumber>("IDC_NUMBER_CURRENT_CHANNEL_PAGE");
    const auto container         = FindChild<Gx::UiContainer>("IDC_CONTAINER_CHANNEL_CONTROLS");
    const auto channelList       = container->FindChild<Gx::List>("IDC_LIST_CHANNEL");

    m_channelPageIndex = page;
    currentPageNumber->SetValue(m_channelPageIndex);

    const int start = (page - 1) * m_channelsPerPage;
    int end = start + m_channelsPerPage;
    if (end > m_planetInfo.Channels.size())
        end = m_planetInfo.Channels.size();

    const auto children = channelList->GetChildren();
    channelList->SetVisible(true);
    for (unsigned int i = 0; i < m_channelsPerPage; i++)
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

    auto currentPageNumber = FindChild<Gx::BitmapNumber>("IDC_NUMBER_CURRENT_CHANNEL_PAGE");
    auto notice            = FindChild<Gx::Image>("IDC_IMAGE_CHANNEL_NOTICE");

    m_noticePageIndex = page;
    currentPageNumber->SetValue(m_noticePageIndex);
    notice->SetFrame(m_noticePageIndex - 1);
}

void ChannelBoard::Update(const double delta)
{
    Control::Update(delta);
    TaskContainer::Update(delta);
}

Gx::RenderStates ChannelBoard::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
{
    if (m_animationEnabled)
        surface.Render(m_captureImage, states);

    return Image::Render(surface, states);
}

void ChannelBoard::Invalidate()
{
    Gx::Image::Invalidate();
}
