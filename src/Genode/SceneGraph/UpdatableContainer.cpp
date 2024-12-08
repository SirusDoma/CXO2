#include <Genode/SceneGraph/UpdatableContainer.hpp>

namespace Gx
{
    void UpdatableContainer::Update(const double delta)
    {
        const auto updatables = GetChildren();
        for (const auto node : updatables)
        {
            if (const auto updatable = dynamic_cast<Updatable*>(node))
                updatable->Update(delta);
        }
    }
}
