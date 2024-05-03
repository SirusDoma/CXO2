#ifndef GENODE_SCENEGRAPH_RENDERABLE_CONTAINER_HPP
#define GENODE_SCENEGRAPH_RENDERABLE_CONTAINER_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/Entities/Renderable.hpp>

namespace Gx
{
    class RenderableContainer : public virtual Node, public virtual Renderable
    {
    public:
        ~RenderableContainer() override = default;

    protected:
        RenderableContainer() = default;

        RenderStates Render(RenderSurface &surface, RenderStates states) const  override;
        virtual bool IsBatchLevelConstrained() const;
    };
}

#endif
