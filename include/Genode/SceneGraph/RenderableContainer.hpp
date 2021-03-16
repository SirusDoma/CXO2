#ifndef GENODE_RENDERABLE_CONTAINER_HPP
#define GENODE_RENDERABLE_CONTAINER_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/Entities/Renderable.hpp>

namespace Gx
{
    class RenderableContainer : public virtual Node, public Renderable
    {
    public:
        virtual ~RenderableContainer();

    protected:
        RenderableContainer();
        sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;
    };
}

#endif
