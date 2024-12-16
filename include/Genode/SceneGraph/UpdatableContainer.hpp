#pragma once

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/Entities/Updatable.hpp>

namespace Gx
{
    class UpdatableContainer : public virtual Node, public virtual Updatable
    {
    protected:
        UpdatableContainer() = default;
        void Update(double delta) override;
    };
}
