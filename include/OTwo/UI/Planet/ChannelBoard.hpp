#ifndef O2JAM_PLANET_CHANNELBOARD_HPP
#define O2JAM_PLANET_CHANNELBOARD_HPP

#include <OTwo/Models/Planet.hpp>
#include <OTwo/UI/Planet/ChannelButton.hpp>

#include <Genode/Audio/Mixer.hpp>
#include <Genode/SceneGraph.hpp>
#include <Genode/Fx/Move.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/BitmapNumber.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/UiContainer.hpp>
#include <Genode/UI/List.hpp>

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <functional>

class ChannelBoard : public Gx::Image, public Gx::TaskContainer
{
public:
    enum class Tab
    {
        Notice,
        ChannelList
    };

    ChannelBoard(Gx::Mixer& mixer, Gx::ResourceManager& resources);

    void Initialize() override;
    sf::FloatRect GetLocalBounds() const override;

    void Show(MusicHall hall, std::function<void()> callback);
    void UpdateChannelList(const PlanetInfo& planet);
    void ShowChannelList(unsigned int page);
    void ShowNotice(unsigned int page);
    void SwitchTab(Tab tab);

    bool InTransition() const;

    bool IsAnimationEnabled() const;
    void SetAnimationEnabled(bool animationEnabled);

    void SetChannelEnterCallback(std::function<void(MusicHall, ServerChannel)> callback);

    unsigned int GetChannelsPerPage() const;
    void SetChannelsPerPage(unsigned int channelsPerPage);

    void SetChannelButton(ChannelButton& button);

private:
    void CaptureCurrentState();

    void Update(double delta) override;
    Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

    void Invalidate() override;

    Gx::Mixer& m_mixer;
    Gx::ResourceManager& m_resources;

    ChannelButton* m_channelButton;
    Gx::Image m_captureImage;
    sf::RenderTexture m_renderTexture;

    PlanetInfo m_planetInfo;
    ChannelBoard::Tab m_tab;
    std::function<void(MusicHall, ServerChannel)> m_callback;

    Gx::Sequence m_sequence;
    Gx::Move m_moveIn, m_moveOut;

    bool m_transitioning, m_animationEnabled;
    int m_selectedChannel;
    unsigned int m_channelsPerPage, m_channelPageIndex, m_channelMaxPage, m_noticePageIndex, m_noticeMaxPage;
};

#endif
