
#ifndef GENODE_DEFINITION_LOADER_HPP
#define GENODE_DEFINITION_LOADER_HPP

#include <Genode/IO/ResourceLoader.hpp>
#include <Genode/IO/ResourceDefinition.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/System/Primitives.hpp>

#include <memory>

namespace Gx
{
    template<typename T>
    class DefinitionLoader : public priv::BaseLoader
    {
    public:
        virtual ~DefinitionLoader() {}

        virtual ResourceDefinition* Load(Uint8* data, Uint64 size) const = 0;
        virtual T* Create(ResourceDefinition* definition, ResourceContext context) const = 0;
    };
}

#endif