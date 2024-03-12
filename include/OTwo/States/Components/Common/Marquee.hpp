#ifndef O2JAM_ROOM_MARQUEE_HPP
#define O2JAM_ROOM_MARQUEE_HPP

#include <Genode/UI/Control.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/Graphics/Sprite.hpp>

class Marquee : public Gx::Label
{
public:
    Marquee();
    Marquee(const Marquee &copy);

    using Gx::Label::Label;

    virtual const sf::FloatRect GetLocalBounds() const;
    void SetLocalBounds(const sf::FloatRect &bounds);

    double GetSpeed() const;
    void SetSpeed(double speed);

private:
    virtual void Update(double delta);
    virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;

    virtual void Invalidate();

    sf::FloatRect m_bounds;
    Gx::Sprite m_sprite;
    double m_speed;

    mutable std::unique_ptr<sf::RenderTexture> m_renderTexture;
};

#endif
