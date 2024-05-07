#include <Genode/SceneGraph/RenderableContainer.hpp>

namespace Gx
{
    RenderStates RenderableContainer::Render(RenderSurface &surface, RenderStates states) const
    {
        for (const auto node : GetChildren())
        {
            states.BatchLevel += 1.0f;
            if (const auto renderable = dynamic_cast<Renderable*>(node))
                renderable->Render(surface, states);
        }

        return states;
    }
}
