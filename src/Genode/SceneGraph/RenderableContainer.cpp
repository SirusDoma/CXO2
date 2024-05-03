#include <Genode/SceneGraph/RenderableContainer.hpp>

namespace Gx
{
    RenderStates RenderableContainer::Render(RenderSurface &surface, RenderStates states) const
    {
        float level  = states.BatchLevel;
        for (const auto node : GetChildren())
        {
            states.BatchLevel += 1.f;
            if (const auto renderable = dynamic_cast<Renderable*>(node))
                renderable->Render(surface, states);

            if (IsBatchLevelConstrained())
                states.BatchLevel = level;
        }

        return states;
    }

    bool RenderableContainer::IsBatchLevelConstrained() const
    {
        return false;
    }
}
