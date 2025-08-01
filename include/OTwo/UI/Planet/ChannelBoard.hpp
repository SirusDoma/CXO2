#pragma once

#include <OTwo/Models/Planet.hpp>
#include <OTwo/Messages/Responses/ChannelListResponse.hpp>
#include <OTwo/UI/Planet/ChannelButton.hpp>
#include <OTwo/Services/PlanetService.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/SceneGraph/TaskContainer.hpp>
#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/UiContainer.hpp>

#include <SFML/Graphics/RenderTexture.hpp>

#include <functional>

class ChannelBoard : public Gx::Image, public Gx::TaskContainer
{
public:
    enum class Tab
    {
        Notice,
        ChannelList
    };

    ChannelBoard(Gx::AudioMixer& mixer, Gx::ResourceManager& resources);

    void Initialize() override;
    sf::FloatRect GetLocalBounds() const override;

    void Show(MusicHall hall, std::function<void()> callback);
    void UpdateChannelList(MusicHall hall, const ChannelListResponse& response);
    void ShowChannelList(unsigned int page);
    void ShowNotice(unsigned int page);
    void SwitchTab(Tab tab);

    bool InTransition() const;

    bool IsAnimationEnabled() const;
    void SetAnimationEnabled(bool animationEnabled);

    void SetChannelEnterCallback(std::function<void(MusicHall, std::uint16_t, std::uint16_t)> callback);

    unsigned int GetChannelsPerPage() const;
    void SetChannelsPerPage(unsigned int channelsPerPage);

    void SetChannelButton(ChannelButton& button);

private:
    void CaptureCurrentState();

    void Update(double delta) override;
    Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

    void Invalidate() override;

    Gx::AudioMixer& m_mixer;
    Gx::ResourceManager& m_resources;

    ChannelButton* m_channelButton;
    Gx::Image m_captureImage;
    sf::RenderTexture m_renderTexture;
    Gx::Sequence m_sequence;

    ::ChannelBoard::Tab m_tab;
    MusicHall m_hall{MusicHall::None};

    std::vector<ChannelListResponse::ChannelState> m_channels;
    std::function<void(MusicHall, std::uint16_t, std::uint16_t)> m_callback;

    bool m_transitioning, m_animationEnabled;
    int m_selectedChannel;
    unsigned int m_channelsPerPage, m_channelPageIndex, m_channelMaxPage, m_noticePageIndex, m_noticeMaxPage;
};
