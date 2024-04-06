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

#include <OTwo/Data/Planet.hpp>

#include <functional>
#include "ChannelButton.hpp"

class ChannelBoard : public Gx::Image, public Gx::TaskContainer
{
public:
    constexpr static const unsigned int CHANNEL_LIST_PER_PAGE = 20;
    enum class Tab
    {
        Notice,
        ChannelList
    };

    ChannelBoard();

    void Initialize() override;
    sf::FloatRect GetLocalBounds() const override;

    void Show(MusicHall hall, std::function<void()> callback);
    void UpdateChannelList(Planet planet);
    void ShowChannelList(unsigned int page);
    void ShowNotice(unsigned int page);
    void SwitchTab(Tab tab);

    bool InTransition() const;

    bool IsAnimationEnabled() const;
    void SetAnimationEnabled(bool animationEnabled);

    void SetChannelEnterCallback(std::function<void(MusicHall, Channel)> callback);

    unsigned int GetChannelsPerPage() const;
    void SetChannelsPerPage(unsigned int channelsPerPage);

    void SetChannelButton(ChannelButton &button);

private:
    void CaptureCurrentState();

    void Update(double delta) override;
    Gx::RenderStates Render(sf::RenderTarget &target, Gx::RenderStates states) const override;

    void Invalidate() override;

    ChannelButton *m_channelButton;
    Gx::Image m_duplicateImage;
    sf::RenderTexture m_renderTexture;

    Planet m_planet;
    ChannelBoard::Tab m_tab;
    std::function<void(MusicHall, Channel)> m_callback;

    bool m_transitioning, m_animationEnabled;
    int m_selectedChannel;
    unsigned int m_channelsPerPage, m_channelPageIndex, m_channelMaxPage, m_noticePageIndex, m_noticeMaxPage;
};

#endif
