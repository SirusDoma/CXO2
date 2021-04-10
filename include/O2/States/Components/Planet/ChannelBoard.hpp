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

class ChannelBoard : public virtual Gx::Control, public virtual Gx::TaskContainer
{
public:

    enum Tab
    {
        Notice,
        ChannelList
    };

    ChannelBoard();
    void Initialize(Gx::Scene &scene);

    virtual const sf::FloatRect GetLocalBounds() const;

    void Show(Planet planet, std::function<void()> callback);
    void UpdateChannelList(PlanetInfo info);
    void ShowChannelList(int page);
    void ShowNotice(int page);
    void SwitchTab(Tab tab);

    bool InTransition() const;
    void SetEnterChannelCallback(std::function<void(Planet, ChannelInfo)> callback);

private:
    constexpr static const unsigned int CHANNEL_LIST_PER_PAGE = 20;

    void CaptureCurrentState();

    virtual void Update(double delta);
    virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;

    virtual void Invalidate();

    Gx::Image       *m_background, *m_notice, *m_channelCategory, m_duplicateImage;
    Gx::Button      *m_channelTabButton, *m_noticeTabButton;
    Gx::UiContainer *m_channelListContainer;
    Gx::List        *m_list;
    Gx::Number      *m_currentPageNumber, *m_maxPageNumber;

    Gx::Scene         *m_scene;
    sf::SoundSource   *m_sfxPopup, *m_sfxNavigate, *m_sfxEnter;
    sf::RenderTexture m_renderTexture;

    sf::Vector2f m_position;
    PlanetInfo m_planetInfo;
    Tab m_tab;

    bool m_animating;
    int m_selectedChannel, m_channelPageIndex, m_channelMaxPage, m_noticePageIndex, m_noticeMaxPage;
    std::function<void(Planet, ChannelInfo)> m_callback;
    std::vector<std::unique_ptr<ChannelButton>> m_channelButtons;
};

#endif
