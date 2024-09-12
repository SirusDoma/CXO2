#ifndef O2JAM_ROOM_MARQUEE_HPP
#define O2JAM_ROOM_MARQUEE_HPP

#include <Genode/UI/Control.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/Graphics/Sprite.hpp>

class Marquee : public Gx::Label
{
public:
    Marquee();
    Marquee(const Marquee& copy);

    using Gx::Label::Label;

    sf::FloatRect GetLocalBounds() const override;
    void SetLocalBounds(const sf::FloatRect& bounds);

    double GetSpeed() const;
    void SetSpeed(double speed);

private:
    void Update(double delta) override;
    Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

    void Invalidate() override;

    sf::FloatRect m_bounds;
    Gx::Sprite m_sprite;
    double m_speed;

    mutable std::unique_ptr<sf::RenderTexture> m_renderTexture;
};

#endif
