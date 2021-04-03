#include <Genode/SceneGraph/UpdatableContainer.hpp>

namespace Gx
{
    UpdatableContainer::UpdatableContainer()
    {
    }

    UpdatableContainer::~UpdatableContainer()
    {
    }

    void UpdatableContainer::Update(double delta)
    {
        for (auto node : GetChildren())
        {
            auto updatable = dynamic_cast<Updatable*>(node);
            if (updatable)
                updatable->Update(delta);
        }
    }
}
