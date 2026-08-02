#pragma once

#include <Genode/UI/Control.hpp>
#include <Genode/UI/Label.hpp>

#include <SFML/Graphics/VertexArray.hpp>

namespace Cx
{
    class Marquee : public Gx::Label
    {
    public:
        Marquee();
        Marquee(const Marquee& copy);

        using Gx::Label::Label;

        sf::FloatRect GetLocalBounds() const override;
        void SetLocalBounds(const sf::FloatRect& bounds) override;

        double GetSpeed() const;
        void SetSpeed(double speed);

    private:
        void Update(const sf::Time& delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        void OnGeometryUpdated() const override;

        sf::FloatRect m_bounds;
        double m_speed;

        mutable sf::VertexArray m_unclippedVertices;
        mutable sf::VertexArray m_unclippedOutlineVertices;
        mutable bool m_unclippedStale{true};
    };
}
