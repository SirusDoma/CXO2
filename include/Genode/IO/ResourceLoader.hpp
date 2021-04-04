#ifndef GENODE_IO_RESOURCE_LOADER_HPP
#define GENODE_IO_RESOURCE_LOADER_HPP

#include <Genode/System/Primitives.hpp>

#include <Genode/IO/ResourceMetadata.hpp>
#include <Genode/IO/ResourceContext.hpp>

#include <memory>

namespace Gx
{
    struct ResourceContext;
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

        virtual bool IsMetadataRequired() const = 0;
        virtual bool IsResourceStream() const { return false; }

        virtual std::unique_ptr<ResourceMetadata> LoadMetadata(const void* data, std::size_t size) const = 0;

        virtual ResourcePtr<T> Load(const ResourceMetadata& metadata, const ResourceContext& context = ResourceContext()) const = 0;
        virtual ResourcePtr<T> Load(const void* data, std::size_t size) const = 0;
    };
}

#endif