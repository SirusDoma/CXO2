#ifndef O2JAM_PLANET_CHANNELBOARD_HPP
#define O2JAM_PLANET_CHANNELBOARD_HPP

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <Genode/SceneGraph.hpp>
#include <Genode/Graphics/Sprite.hpp>

#include <O2/Data/Planet.hpp>

#include <functional>

class ChannelBoard : public virtual Gx::Node, public virtual Gx::TaskContainer, public virtual Gx::Renderable, public virtual Gx::Inputable
{
public:
    enum Tab
    {
        Notice,
        ChannelList
    };

    ChannelBoard();
    ~ChannelBoard();

    void Show(Planet planet, std::function<void()> callback);
    void SwitchTab(Tab tab);

    bool InTransition() const;

private:
    void Initialize();
    virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;

    sf::Vector2f m_position;
    sf::IntRect  m_texCoords;

    Gx::Sprite *m_background;
    Gx::Sprite *m_notice;

    sf::Sound       m_sfx;
    sf::SoundBuffer m_sfxBuffer;

    Planet m_planet;
    Tab m_tab;
    bool m_animating;
};

#endif
