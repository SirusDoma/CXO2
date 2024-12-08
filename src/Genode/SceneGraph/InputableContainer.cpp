#include <Genode/SceneGraph/InputableContainer.hpp>

namespace Gx
{
    bool InputableContainer::Input(const sf::Event& ev)
    {
        if (Inputable::Input(ev))
        {
            const auto inputables = GetChildren();
            for (const auto node : inputables)
            {
                if (const auto inputable = dynamic_cast<Inputable *>(node); inputable)
                    inputable->Input(ev);
            }

            return true;
        }

        return false;
    }
}
