#include <CXO2/UI/Planet/ChannelBoard.hpp>
#include <CXO2/UI/Planet/ChannelButton.hpp>

#include <CXO2/States/State.hpp>
#include <CXO2/Services/PlanetService.hpp>

#include <CXO2/Constants/Identifiers/Sound.hpp>
#include <CXO2/Constants/Identifiers/Planet.hpp>

#include <Genode/UI/List.hpp>
#include <Genode/Tasks.hpp>
#include <Genode/Tween.hpp>

#include <cmath>

namespace Cx
{
    using namespace Constants::Identifiers;

    ChannelBoard::ChannelBoard(Gx::AudioMixer& mixer, Gx::ResourceManager& resources) :
        m_mixer(mixer),
        m_resources(resources),
        m_channelButton(),
        m_captureImage(),
        m_sequence(),
        m_tab(Tab::Notice),
        m_transitioning(false),
        m_animationEnabled(false),
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

        m_sfxNavigate        = &m_resources.AddFromFile<sf::Sound>(Sound::Effects::EF_07);
        m_sfxEnter           = &m_resources.AddFromFile<sf::Sound>(Sound::Effects::EF_10);
        const auto container = FindChild<Gx::UiContainer>(Resource::Planet::ChannelBoard::IDC_CONTAINER_CHANNEL_CONTROLS);

        const auto channelTabButton = FindChild<Gx::Button>(Resource::Planet::ChannelBoard::IDC_BUTTON_CHANNEL_TAB);
        channelTabButton->SetEnabled(false);
        channelTabButton->SetClickCallback([this] (auto& sender, auto& ev)
        {
            OnChannelTabButtonClicked(sender, ev);
        });

        const auto noticeTabButton = FindChild<Gx::Button>(Resource::Planet::ChannelBoard::IDC_BUTTON_NOTICE_TAB);
        noticeTabButton->SetClickCallback([this] (auto& sender, auto& ev)
        {
            OnNoticeTabButtonClicked(sender, ev);
        });

        const auto channelList       = container->FindChild<Gx::List>(Resource::Planet::ChannelBoard::IDC_LIST_CHANNEL);
        const auto currentPageNumber = FindChild<Gx::BitmapNumber>(Resource::Planet::ChannelBoard::IDC_NUMBER_CURRENT_CHANNEL_PAGE);
        const auto maxPageNumber     = FindChild<Gx::BitmapNumber>(Resource::Planet::ChannelBoard::IDC_NUMBER_MAX_CHANNEL_PAGE);
        currentPageNumber->SetDigitCount(2);
        maxPageNumber->SetDigitCount(2);

        const auto enterButton = container->FindChild<Gx::Button>(Resource::Planet::ChannelBoard::IDC_BUTTON_CHANNEL_ENTER);
        enterButton->SetClickCallback([this] (auto& sender, auto& ev)
        {
            OnChannelEnterButtonClicked(sender, ev);
        });

        const auto channelButtons = channelList->GetChildren();
        for (std::size_t i = 0; i < channelButtons.size(); i++)
        {
            const auto channelButton = dynamic_cast<ChannelButton*>(channelButtons[i]);
            channelButton->SetChannelNumber(i + 1);
            channelButton->SetClickCallback([this] (auto& sender, auto& ev)
            {
                OnChannelButtonClicked(sender, ev);
            });

            channelButton->SetDoubleClickCallback([this] (auto& sender, auto& ev)
            {
                OnChannelButtonDoubleClicked(sender, ev);
            });
        }

        const auto leftButton = FindChild<Gx::Button>(Resource::Planet::ChannelBoard::IDC_BUTTON_CHANNEL_LEFT);
        leftButton->SetClickCallback([this] (auto& sender, auto& ev)
        {
            OnChannelLeftButtonClicked(sender, ev);
        });

        const auto rightButton = FindChild<Gx::Button>(Resource::Planet::ChannelBoard::IDC_BUTTON_CHANNEL_RIGHT);
        rightButton->SetClickCallback([this] (auto& sender, auto& ev)
        {
            OnChannelRightButtonClicked(sender, ev);
        });

        const auto notice = FindChild<Image>(Resource::Planet::ChannelBoard::IDC_IMAGE_CHANNEL_NOTICE);
        m_noticePageIndex = 1;
        m_noticeMaxPage   = notice->GetFrameCount();

        if (const auto bounds = GetLocalBounds(); !m_renderTexture.resize(sf::Vector2u(bounds.size.x, bounds.size.y)))
            throw Gx::Exception("Failed to create render texture");

        m_renderTexture.setSmooth(true);
        m_captureImage.SetOrigin(GetOrigin());
        m_captureImage.SetPosition(GetPosition());
        m_captureImage.SetVisible(false);

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
        const auto bounds = Image::GetLocalBounds();
        if (bounds != sf::FloatRect())
            return bounds;

        for (std::size_t i = 0; i < GetFrameCount(); i++)
        {
            if (GetFrame(i)->TexCoords != sf::IntRect())
            {
                return sf::FloatRect{
                    {},
                    {
                        static_cast<float>(GetFrame(i)->TexCoords.size.x),
                        static_cast<float>(GetFrame(i)->TexCoords.size.y),
                    }
                };
            }
        }

        return bounds;
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

    void ChannelBoard::SetChannelEnterCallback(std::function<void(MusicHall, std::uint16_t, std::uint16_t)> callback)
    {
        m_callback = std::move(callback);
    }

    void ChannelBoard::CaptureCurrentState()
    {
        m_renderTexture.clear(sf::Color::Transparent);
        {
            auto adapter = Gx::RenderSurfaceAdaptor(m_renderTexture);
            Render(adapter, sf::Transform().translate(-GetPosition()));
        }
        m_renderTexture.display();

        m_captureImage.SetTexture(m_renderTexture.getTexture());
        m_captureImage.SetVisible(true);
    }

    void ChannelBoard::SwitchTab(const Tab tab)
    {
        m_tab = tab;

        const auto notice            = FindChild<Image>(Resource::Planet::ChannelBoard::IDC_IMAGE_CHANNEL_NOTICE);
        const auto channelTabButton  = FindChild<Gx::Button>(Resource::Planet::ChannelBoard::IDC_BUTTON_CHANNEL_TAB);
        const auto noticeTabButton   = FindChild<Gx::Button>(Resource::Planet::ChannelBoard::IDC_BUTTON_NOTICE_TAB);
        const auto container         = FindChild<Gx::UiContainer>(Resource::Planet::ChannelBoard::IDC_CONTAINER_CHANNEL_CONTROLS);
        const auto currentPageNumber = FindChild<Gx::BitmapNumber>(Resource::Planet::ChannelBoard::IDC_NUMBER_CURRENT_CHANNEL_PAGE);
        const auto maxPageNumber     = FindChild<Gx::BitmapNumber>(Resource::Planet::ChannelBoard::IDC_NUMBER_MAX_CHANNEL_PAGE);

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

            if (m_hall != MusicHall::None)
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
        if (m_transitioning || m_hall == hall)
            return;

        const auto sfxPopup    = m_resources.Find<sf::Sound>("Planet/openChannel");
        const auto container   = FindChild<Gx::UiContainer>(Resource::Planet::ChannelBoard::IDC_CONTAINER_CHANNEL_CONTROLS);
        const auto category    = container->FindChild<Image>(Resource::Planet::ChannelBoard::IDC_IMAGE_CHANNEL_CATEGORY);
        const auto channelList = container->FindChild<Gx::List>(Resource::Planet::ChannelBoard::IDC_LIST_CHANNEL);

        if (m_animationEnabled)
            CaptureCurrentState();

        m_hall = hall;
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

            m_sequence = Gx::Sequence([&, callback]
                {
                    m_transitioning = false;
                    m_captureImage.SetVisible(false);
                    if (callback)
                        callback();
                },
                Gx::Move(*this, position - sf::Vector2f(30, 0), sf::milliseconds(200)),
                Gx::Move(*this, position, sf::milliseconds(200))
            );

            Run(m_sequence);
        }
        else
        {
            m_transitioning = false;
            if (callback)
                callback();
        }
    }

    void ChannelBoard::UpdateChannelList(const MusicHall hall, const ChannelListResponse& response)
    {
        const auto container   = FindChild<Gx::UiContainer>(Resource::Planet::ChannelBoard::IDC_CONTAINER_CHANNEL_CONTROLS);
        const auto channelList = container->FindChild<Gx::List>(Resource::Planet::ChannelBoard::IDC_LIST_CHANNEL);

        m_hall = hall;
        m_channels = response.Channels.GetContainer();
        if (channelList->GetChildrenCount() == 0)
            m_channelMaxPage = 1;
        else
            m_channelMaxPage = static_cast<int>(std::ceil(static_cast<float>(m_channels.size()) / channelList->GetChildrenCount()));

        const auto maxPageNumber = FindChild<Gx::BitmapNumber>(Resource::Planet::ChannelBoard::IDC_NUMBER_MAX_CHANNEL_PAGE);
        maxPageNumber->SetValue(m_channelMaxPage);

        m_selectedChannel = 0;
        ShowChannelList(1);
        SetEnabled(true);
    }

    void ChannelBoard::ShowChannelList(unsigned int page)
    {
        if (page <= 0)
            page = 1;

        if (page > m_channelMaxPage)
            page = m_channelMaxPage;

        const auto currentPageNumber = FindChild<Gx::BitmapNumber>(Resource::Planet::ChannelBoard::IDC_NUMBER_CURRENT_CHANNEL_PAGE);
        const auto container         = FindChild<Gx::UiContainer>(Resource::Planet::ChannelBoard::IDC_CONTAINER_CHANNEL_CONTROLS);
        const auto channelList       = container->FindChild<Gx::List>(Resource::Planet::ChannelBoard::IDC_LIST_CHANNEL);

        m_channelPageIndex = page;
        currentPageNumber->SetValue(m_channelPageIndex);

        const int start = (page - 1) * m_channelsPerPage;
        int end = start + m_channelsPerPage;
        if (end > static_cast<int>(m_channels.size()))
            end = static_cast<int>(m_channels.size());

        const auto children = channelList->GetChildren();
        channelList->SetVisible(true);
        for (unsigned int i = 0; i < m_channelsPerPage; i++)
        {
            if (i >= children.size())
                break;

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

            const auto& channel = m_channels[channelIndex];
            if (!channel.Active)
            {
                channelButton->SetVisible(false);
                channelButton->SetEnabled(false);

                continue;
            }

            channelButton->SetChannelNumber(channelIndex + 1);
            channelButton->SetMusicHall(m_hall);
            channelButton->SetCheckedState(channelIndex == m_selectedChannel);
            channelButton->SetChannelPopulation(channel.UserCount, channel.Capacity);
            channelButton->SetVisible(true);
            channelButton->SetEnabled(true);
            m_channelButtonIndices[channelButton] = channelIndex;
            channelButton->SetClickCallback([this] (auto& sender, auto& ev)
            {
                OnChannelClicked(sender, ev);
            });
        }
    }

    void ChannelBoard::ShowNotice(unsigned int page)
    {
        if (page <= 0)
            page = 1;

        if (page > m_noticeMaxPage)
            page = m_noticeMaxPage;

        auto currentPageNumber = FindChild<Gx::BitmapNumber>(Resource::Planet::ChannelBoard::IDC_NUMBER_CURRENT_CHANNEL_PAGE);
        auto notice            = FindChild<Image>(Resource::Planet::ChannelBoard::IDC_IMAGE_CHANNEL_NOTICE);

        m_noticePageIndex = page;
        currentPageNumber->SetValue(m_noticePageIndex);
        notice->SetFrame(m_noticePageIndex - 1);
    }

    void ChannelBoard::OnChannelTabButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        if (m_tab == Tab::ChannelList || m_transitioning || m_hall == MusicHall::None)
        {
            ev.Handled = true;
            return;
        }

        SwitchTab(Tab::ChannelList);
    }

    void ChannelBoard::OnNoticeTabButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        if (m_tab == Tab::Notice || m_transitioning)
        {
            ev.Handled = true;
            return;
        }

        SwitchTab(Tab::Notice);
    }

    void ChannelBoard::OnChannelEnterButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        m_mixer.Play(*m_sfxEnter, Sound::Channel::SFX);
        if (m_callback && m_selectedChannel >= 0 && m_selectedChannel < static_cast<int>(m_channels.size()))
            m_callback(m_hall, m_channels[m_selectedChannel].GatewayID, m_channels[m_selectedChannel].ID);
    }

    void ChannelBoard::OnChannelButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto channelButton = dynamic_cast<ChannelButton*>(&sender);
        m_selectedChannel = channelButton->GetChannelNumber() - 1;
    }

    void ChannelBoard::OnChannelButtonDoubleClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        const auto container   = FindChild<Gx::UiContainer>(Resource::Planet::ChannelBoard::IDC_CONTAINER_CHANNEL_CONTROLS);
        const auto enterButton = container->FindChild<Gx::Button>(Resource::Planet::ChannelBoard::IDC_BUTTON_CHANNEL_ENTER);

        enterButton->PerformClick();
    }

    void ChannelBoard::OnChannelLeftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        m_mixer.Play(*m_sfxNavigate, Sound::Channel::SFX);
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

    void ChannelBoard::OnChannelRightButtonClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        m_mixer.Play(*m_sfxNavigate, Sound::Channel::SFX);
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

    void ChannelBoard::OnChannelClicked(Gx::Control& sender, Gx::Control::Event& ev)
    {
        m_selectedChannel = m_channelButtonIndices.at(&sender);
    }

    void ChannelBoard::Update(const sf::Time& delta)
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
        Image::Invalidate();
    }
}
