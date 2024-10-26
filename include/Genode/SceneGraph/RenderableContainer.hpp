#pragma once

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/Entities/Renderable.hpp>

namespace Gx
{
    class RenderableContainer : public virtual Node, public virtual Renderable
    {
    protected:
        RenderableContainer() = default;

        RenderStates Render(RenderSurface& surface, RenderStates states) const override;
    };
}
