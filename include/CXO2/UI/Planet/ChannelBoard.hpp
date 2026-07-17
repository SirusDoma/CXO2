#pragma once

#include <CXO2/Models/Planet.hpp>
#include <CXO2/Network/Responses/ChannelListResponse.hpp>
#include <CXO2/UI/Planet/ChannelButton.hpp>
#include <CXO2/Services/PlanetService.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/SceneGraph/TaskContainer.hpp>
#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Tasks/Sequence.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/UiContainer.hpp>

#include <SFML/Graphics/RenderTexture.hpp>

#include <functional>
#include <unordered_map>

namespace sf
{
    class Sound;
}

namespace Cx
{
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

        void OnChannelTabButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnNoticeTabButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnChannelEnterButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnChannelButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnChannelButtonDoubleClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnChannelLeftButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnChannelRightButtonClicked(Gx::Control& sender, Gx::Control::Event& ev);
        void OnChannelClicked(Gx::Control& sender, Gx::Control::Event& ev);

        void Update(const sf::Time& delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        void Invalidate() override;

        Gx::AudioMixer& m_mixer;
        Gx::ResourceManager& m_resources;
        sf::Sound* m_sfxNavigate{nullptr};
        sf::Sound* m_sfxEnter{nullptr};

        ChannelButton* m_channelButton;
        Gx::Image m_captureImage;
        sf::RenderTexture m_renderTexture;
        Gx::Sequence m_sequence;

        Cx::ChannelBoard::Tab m_tab;
        MusicHall m_hall{MusicHall::None};

        std::vector<ChannelListResponse::ChannelState> m_channels;
        std::function<void(MusicHall, std::uint16_t, std::uint16_t)> m_callback;
        std::unordered_map<Gx::Control*, int> m_channelButtonIndices;

        bool m_transitioning, m_animationEnabled;
        int m_selectedChannel;
        unsigned int m_channelsPerPage, m_channelPageIndex, m_channelMaxPage, m_noticePageIndex, m_noticeMaxPage;
    };
}
