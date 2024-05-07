#ifndef GENODE_ENTITIES_RENDERABLE_HPP
#define GENODE_ENTITIES_RENDERABLE_HPP

#include <Genode/Graphics/RenderSurface.hpp>
#include <Genode/Graphics/RenderStates.hpp>
#include <Genode/Graphics/RenderTargetAdapter.hpp>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

namespace Gx
{
    class Renderable : public sf::Drawable
    {
    public:
        Renderable() = default;
        ~Renderable() override = default;

        virtual RenderStates Render(RenderSurface &surface, RenderStates states) const = 0;

    protected:
        void draw(sf::RenderTarget& target, const sf::RenderStates states) const override
        {
            auto adapter = RenderTargetAdapter(target);
            Render(adapter, RenderStates(states));
        }
    };
}

#endif
