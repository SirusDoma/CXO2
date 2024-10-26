#pragma once

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/Entities/Updatable.hpp>

namespace Gx
{
    class UpdatableContainer : public virtual Node, public virtual Updatable
    {
    public:
        ~UpdatableContainer() override;

    protected:
        UpdatableContainer();
        void Update(const double delta) override;
    };
}
