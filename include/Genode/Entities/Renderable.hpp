#ifndef GENODE_ENTITIES_RENDERABLE_HPP
#define GENODE_ENTITIES_RENDERABLE_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

namespace Gx
{
    class Renderable : public virtual sf::Drawable
    {
    public:
        Renderable() {};
        virtual ~Renderable() {};
        virtual sf::RenderStates Render(sf::RenderTarget& target, sf::RenderStates states) const = 0;

    protected:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const { Render(target, states); }
    };
}

#endif
