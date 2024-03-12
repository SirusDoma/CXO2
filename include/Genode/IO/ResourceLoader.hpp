#ifndef GENODE_IO_RESOURCE_LOADER_HPP
#define GENODE_IO_RESOURCE_LOADER_HPP

#include <Genode/IO/Resource.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <SFML/System/InputStream.hpp>

namespace Gx
{
    template<typename T>
    class ResourceLoader
    {
    public:
        ResourceLoader() = default;
        virtual ~ResourceLoader() = default;

        virtual bool IsStreaming() const { return false; }

        virtual ResourcePtr<T> LoadFromFile(const std::string &fileName, const ResourceContext &ctx) const = 0;
        virtual ResourcePtr<T> LoadFromMemory(void *data, std::size_t size, const ResourceContext &ctx) const = 0;
        virtual ResourcePtr<T> LoadFromStream(sf::InputStream &stream, const ResourceContext &ctx) const = 0;
    };
}

#endif