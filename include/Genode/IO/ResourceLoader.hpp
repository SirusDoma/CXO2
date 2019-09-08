#ifndef GENODE_RESOURCE_LOADER_HPP
#define GENODE_RESOURCE_LOADER_HPP

#include <Genode/System/Primitives.hpp>

namespace Gx
{
    namespace priv
    {
        class BaseLoader
        {
        public:
            virtual ~BaseLoader() {}
        };
    }

    template<typename T>
    class ResourceLoader : public priv::BaseLoader
    {
    public:
        virtual ~ResourceLoader() {}
        virtual T Load(Uint8* data, Uint64 size) const = 0;
    };
}

#endif