#ifndef GENODE_UPDATABLE_CONTAINER_HPP
#define GENODE_UPDATABLE_CONTAINER_HPP

#include <Genode/SceneGraph/Node.hpp>
#include <Genode/Entities/Updatable.hpp>

namespace Gx
{
    class UpdatableContainer : public virtual Node, public virtual Updatable
    {
    public:
        virtual ~UpdatableContainer();

    protected:
        UpdatableContainer();
        virtual void Update(double delta);
    };
}

#endif
