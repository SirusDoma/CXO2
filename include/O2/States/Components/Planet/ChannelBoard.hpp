#ifndef O2JAM_PLANET_CHANNELBOARD_HPP
#define O2JAM_PLANET_CHANNELBOARD_HPP

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <Genode/Audio/Mixer.hpp>
#include <Genode/SceneGraph.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/Number.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/UiContainer.hpp>
#include <Genode/UI/List.hpp>

#include <O2/Data/Planet.hpp>
#include <O2/States/Components/Planet/ChannelButton.hpp>

#include <functional>

class ChannelBoard : public Gx::Image, public Gx::TaskContainer
{
public:
    constexpr static const unsigned int CHANNEL_LIST_PER_PAGE = 20;
    enum Tab
    {
        Notice,
        ChannelList
    };

    ChannelBoard();
    virtual const sf::FloatRect GetLocalBounds() const;

    void Show(Planet::MusicHall hall, std::function<void()> callback);
    void UpdateChannelList(Planet::PlanetInfo info);
    void ShowChannelList(int page);
    void ShowNotice(int page);
    void SwitchTab(Tab tab);

    bool InTransition() const;
    void SetEnterChannelCallback(std::function<void(Planet::MusicHall, Planet::ChannelInfo)> callback);

    unsigned int GetChannelsPerPage() const;
    void SetChannelsPerPage(unsigned int channelsPerPage);

private:
    virtual void Initialize();
    void CaptureCurrentState();

    virtual void Update(double delta);
    virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;

    virtual void Invalidate();

    Gx::Application *m_app;
    Gx::Mixer       *m_mixer;
    sf::SoundSource *m_sfxPopup, *m_sfxNavigate, *m_sfxEnter;

    Gx::Image m_duplicateImage;
    sf::RenderTexture m_renderTexture;

    std::unique_ptr<Gx::UiContainer> m_channelListContainer;
    Gx::ResourcePtr<Gx::Image>  m_notice, m_channelCategory;
    Gx::ResourcePtr<Gx::Button> m_channelTabButton, m_noticeTabButton, m_channelEnterButton, m_navigateLeftButton, m_btnNavigateRightButton;
    Gx::ResourcePtr<Gx::List>   m_channelList;
    Gx::ResourcePtr<Gx::Number> m_currentPageNumber, m_maxPageNumber;

    Planet::PlanetInfo m_planetInfo;
    ChannelBoard::Tab  m_tab;

    std::function<void(Planet::MusicHall, Planet::ChannelInfo)> m_callback;
    std::vector<Gx::ResourcePtr<ChannelButton>>                 m_channelButtons;

    bool         m_animating;
    int          m_selectedChannel;
    unsigned int m_channelsPerPage, m_channelPageIndex, m_channelMaxPage, m_noticePageIndex, m_noticeMaxPage;
};

#endif
