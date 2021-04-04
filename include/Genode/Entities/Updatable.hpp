#ifndef GENODE_ENTITIES_UPDATABLE_HPP
#define GENODE_ENTITIES_UPDATABLE_HPP

namespace Gx
{
    class Updatable
    {
    protected:
        friend class Scene;
        friend class UpdatableContainer;

        virtual void Update(double delta) = 0;
        virtual ~Updatable() {};
    };
}

#endif
