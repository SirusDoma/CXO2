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
        for (const auto node : GetChildren())
        {
            if (const auto updatable = dynamic_cast<Updatable*>(node))
                updatable->Update(delta);
        }
    }
}
