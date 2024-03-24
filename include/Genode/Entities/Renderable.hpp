#ifndef GENODE_ENTITIES_RENDERABLE_HPP
#define GENODE_ENTITIES_RENDERABLE_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <Genode/Graphics/RenderStates.hpp>

namespace Gx
{
    class Renderable : public virtual sf::Drawable
    {
    public:
        Renderable() = default;
        ~Renderable() override = default;
        virtual RenderStates Render(sf::RenderTarget& target, RenderStates states) const = 0;

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override { Render(target, RenderStates(states)); }
    };
}

#endif
