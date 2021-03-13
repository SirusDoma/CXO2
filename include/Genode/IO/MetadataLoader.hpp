
#ifndef GENODE_METADATA_LOADER_HPP
#define GENODE_METADATA_LOADER_HPP

#include <Genode/IO/ResourceLoader.hpp>
#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/System/Primitives.hpp>

#include <memory>

namespace Gx
{
    template<typename T>
    class MetadataLoader : public priv::BaseLoader
    {
    public:
        virtual ~MetadataLoader() {}

        virtual ResourceMetadata* Load(Uint8* data, Uint64 size) const = 0;
        virtual T* Create(ResourceMetadata* definition, ResourceContext context) const = 0;
    };
}

#endif