#ifndef O2JAM_PLANET_CHANNELBOARD_HPP
#define O2JAM_PLANET_CHANNELBOARD_HPP

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <Genode/SceneGraph.hpp>
#include <Genode/UI/Image.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/UI/UiContainer.hpp>

#include <O2/Data/Planet.hpp>

#include <functional>

class ChannelButton;
class ChannelBoard : public virtual Gx::Control, public virtual Gx::TaskContainer
{
public:
    enum Tab
    {
        Notice,
        ChannelList
    };

    ChannelBoard();
    ~ChannelBoard();

    virtual const sf::FloatRect GetLocalBounds() const;

    void Show(Planet planet, std::function<void()> callback);
    void SwitchTab(Tab tab);

    bool InTransition() const;

private:
    void Initialize();
    void LoadSfx();
    virtual void Invalidate();

    virtual void Update(double delta);
    virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;

    sf::Vector2f m_position;

    Gx::Image       *m_background, *m_notice, *m_channelCategory, m_duplicateImage;
    Gx::Button      *m_channelTabButton, *m_noticeTabButton;
    Gx::UiContainer *m_channelListContainer;
    ChannelButton   *m_channelButton;

    sf::Sound         m_showSfx, m_channelNavigateSfx, m_channelEnterSfx;
    sf::SoundBuffer   m_showSfxBuffer, m_channelNavigateSfxBuffer, m_channelEnterSfxBuffer;
    sf::RenderTexture m_duplicateTexture;

    Planet m_planet;
    Tab m_tab;
    bool m_animating;
};

#endif
