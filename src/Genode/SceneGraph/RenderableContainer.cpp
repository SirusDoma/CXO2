#include <Genode/SceneGraph/RenderableContainer.hpp>

namespace Gx
{
    RenderableContainer::RenderableContainer()
    {
    }

    RenderableContainer::~RenderableContainer()
    {
    }

    RenderStates RenderableContainer::Render(sf::RenderTarget &target, RenderStates states) const
    {
        for (auto node : GetChildren())
        {
            auto renderable = dynamic_cast<Renderable*>(node);
            if (renderable)
                renderable->Render(target, states);
        }

        return states;
    }
}
