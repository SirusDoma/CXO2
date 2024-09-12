#ifndef GENODE_SCENEGRAPH_RENDERABLE_CONTAINER_HPP
#define GENODE_SCENEGRAPH_RENDERABLE_CONTAINER_HPP

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

        RenderStates Render(RenderSurface& surface, RenderStates states) const override;
    };
}

#endif
