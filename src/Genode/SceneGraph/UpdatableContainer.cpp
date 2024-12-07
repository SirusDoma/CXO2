#include <Genode/SceneGraph/UpdatableContainer.hpp>

namespace Gx
{
    UpdatableContainer::UpdatableContainer()
    {
    }

    UpdatableContainer::~UpdatableContainer()
    {
    }

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
