#include <Genode/SceneGraph/InputableContainer.hpp>

namespace Gx
{
    InputableContainer::InputableContainer()
    {
    }

    InputableContainer::~InputableContainer()
    {
    }

    bool InputableContainer::Input(sf::Event ev)
    {
        if (Inputable::Input(ev))
        {
            for (auto node : GetChildren())
            {
                auto inputable = dynamic_cast<Inputable *>(node);
                if (inputable)
                    inputable->Input(ev);
            }

            return true;
        }

        return false;
    }
}
