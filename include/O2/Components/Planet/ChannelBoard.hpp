#ifndef O2JAM_PLANET_CHANNELBOARD_HPP
#define O2JAM_PLANET_CHANNELBOARD_HPP

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <Genode/SceneGraph.hpp>
#include <Genode/UI/Image.hpp>

#include <O2/Data/Planet.hpp>

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
    ~ChannelBoard();

    virtual const sf::FloatRect GetLocalBounds() const;

    void Show(Planet planet, std::function<void()> callback);
    void SwitchTab(Tab tab);

    bool InTransition() const;

private:
    void Initialize();
    virtual void Invalidate();

    virtual void Update(double delta);
    virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;

    sf::Vector2f m_position;

    Gx::Image *m_background;
    Gx::Image *m_notice;

    sf::Sound       m_sfx;
    sf::SoundBuffer m_sfxBuffer;

    Planet m_planet;
    Tab m_tab;
    bool m_animating;
};

#endif
