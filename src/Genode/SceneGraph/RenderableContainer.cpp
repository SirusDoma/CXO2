#include <Genode/SceneGraph/RenderableContainer.hpp>

namespace Gx
{
    RenderStates RenderableContainer::Render(RenderSurface& surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        for (const auto node : GetChildren())
        {
            states.Layer += 1.0f;
            if (const auto renderable = dynamic_cast<Renderable*>(node))
                renderable->Render(surface, states);
        }

        return states;
    }
}
