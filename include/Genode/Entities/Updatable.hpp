#ifndef GENODE_ENTITIES_UPDATABLE_HPP
#define GENODE_ENTITIES_UPDATABLE_HPP

namespace Gx
{
    class Updatable
    {
    public:
        virtual void Update(double delta) = 0;
        virtual ~Updatable() = default;
    };
}

#endif
