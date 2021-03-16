#ifndef GENODE_UPDATABLE_HPP
#define GENODE_UPDATABLE_HPP

namespace Gx
{
    class Updatable
    {
    protected:
        friend class UpdatableContainer;

        virtual void Update(double delta) = 0;
        virtual ~Updatable() {};
    };
}

#endif
