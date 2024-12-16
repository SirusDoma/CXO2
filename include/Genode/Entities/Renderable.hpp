#pragma once

#include <Genode/Graphics/RenderSurface.hpp>
#include <Genode/Graphics/RenderStates.hpp>
#include <Genode/Graphics/RenderSurfaceAdaptor.hpp>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>

namespace Gx
{
    class Renderable : public sf::Drawable
    {
    public:
        Renderable() = default;

        virtual RenderStates Render(RenderSurface& surface, RenderStates states) const = 0;

        [[nodiscard]] virtual bool IsVisible() const { return m_visible; }
        virtual void SetVisible(const bool visible) { m_visible = visible; }

    protected:
        void draw(sf::RenderTarget& target, const sf::RenderStates states) const override
        {
            auto adapter = RenderSurfaceAdaptor(target);
            Render(adapter, RenderStates(states));
        }

    private:
        bool m_visible = true;
    };
}
