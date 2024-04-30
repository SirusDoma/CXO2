#include <Genode/SceneGraph/RenderableContainer.hpp>

namespace Gx
{
    RenderableContainer::RenderableContainer()
    {
    }

    RenderableContainer::~RenderableContainer()
    {
    }

    RenderStates RenderableContainer::Render(RenderSurface &surface, RenderStates states) const
    {
        states.BatchLevel += 1.f;
        for (const auto node : GetChildren())
        {
            if (const auto renderable = dynamic_cast<Renderable*>(node))
                renderable->Render(surface, states);
        }

        return states;
    }
}
