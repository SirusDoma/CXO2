#include <Genode/SceneGraph/RenderableContainer.hpp>

namespace Gx
{
    RenderableContainer::RenderableContainer()
    {
    }

    RenderableContainer::~RenderableContainer()
    {
    }

    sf::RenderStates RenderableContainer::Render(sf::RenderTarget &target, sf::RenderStates states) const
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
