#ifndef GENODE_IO_RESOURCEMANAGER_HPP
#define GENODE_IO_RESOURCEMANAGER_HPP

#include <Genode/IO/ResourceContainer.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/IO/ResourceLoaderFactory.hpp>
#include <Genode/IO/ResourceMetadata.hpp>

#include <memory>

namespace Gx
{
    enum ResourceScope { Local, Shared };

    class Node;
    class ResourceManager
    {
    public:
        using ResourceContainerMap = std::unordered_map<std::type_index, std::unique_ptr<priv::BaseContainer>>;
        using ArchiveMap           = std::unordered_map<std::string, std::unique_ptr<Archive>>;
        using EntryMap             = std::unordered_map<std::string, Archive::FileEntry>;

        ResourceManager();
        ~ResourceManager();

        template<typename A>
        A* LoadArchive(const std::string& fileName);

        template<typename R>
        ResourceContainer<R>* Register();

        template<typename R>
        bool Unregister();

        template<typename R>
        ResourcePtr<R> Resolve(const std::string& source);

        template<typename R>
        bool Contains(const std::string& name);

        template<typename R>
        R* Load(const std::string& source);

        template<typename R>
        ResourceMetadata* LoadMetadata(const std::string& source);

        ResourceContext ResolveContext(const ResourceMetadata& metadata);
        Uint64 GetResourceData(const std::string &name, Gx::Uint8 **data) const;

    private:
        template<typename R>
        ResourceContainer<R>* GetContainer();

        ResourceContainerMap m_containers;
        ArchiveMap           m_archives;
        EntryMap             m_entries;
    };
}

#include <Genode/IO/ResourceManager.inl>
#endif
