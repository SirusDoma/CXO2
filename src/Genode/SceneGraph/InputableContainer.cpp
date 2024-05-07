#include <Genode/SceneGraph/InputableContainer.hpp>

namespace Gx
{
    InputableContainer::InputableContainer()
    {
    }

    InputableContainer::~InputableContainer()
    {
    }

    bool InputableContainer::Input(const sf::Event ev)
    {
        if (Inputable::Input(ev))
        {
            for (const auto node : GetChildren())
            {
                if (const auto inputable = dynamic_cast<Inputable *>(node))
                    inputable->Input(ev);
            }

            return true;
        }

        return false;
    }
}
