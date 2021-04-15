#include <Genode/IO/FileHelper.hpp>
#include <Genode/SceneGraph/Node.hpp>
#include "ResourceManager.hpp"


namespace Gx
{
    template<typename A>
    A* ResourceManager::LoadArchive(const std::string& fileName)
    {
        static_assert(std::is_base_of<Archive, A>::value, "Parameter must be a Gx::Archive");

        auto name = FileHelper::GetFileName(fileName);
        auto it = m_archives.find(fileName);
        if (it != m_archives.end())
            return nullptr;

        m_archives[name] = std::make_unique<A>();
        if (!m_archives[name]->Open(FileHelper::GetFullName(fileName)))
        {
            m_archives.erase(m_archives.find(name));
            return nullptr;
        }

        auto prefix = FileHelper::GetFileName(fileName, false) + "/";
        for (auto entry : m_archives[name]->GetFileEntries())
        {
            auto entryName = prefix + entry.Name;
            m_entries[entryName] = entry;
            m_entries[entryName].Parent = m_archives[name].get();
        }

        return static_cast<A*>(m_archives[name].get());
    }

    template<typename R>
    ResourceContainer<R>* ResourceManager::Register()
    {
        auto container = GetContainer<R>();
        if (container)
            return container;

        m_containers[typeid(R)] = std::make_unique<ResourceContainer<R>>();
        return static_cast<ResourceContainer<R>*>(m_containers[typeid(R)].get());
    }

    template<typename R>
    bool ResourceManager::Unregister()
    {
        return m_containers.erase(typeid(R)) != 0;
    }

    template<typename R>
    ResourceContainer<R>* ResourceManager::GetContainer()
    {
        auto iterator = m_containers.find(typeid(R));
        if (iterator != m_containers.end())
            return static_cast<ResourceContainer<R>*>(iterator->second.get());

        return nullptr;
    }

    template<typename R>
    bool ResourceManager::Contains(const std::string &name)
    {
        auto iterator = m_entries.find(name);
        if (iterator != m_entries.end())
            return true;

        auto container = GetContainer<R>();
        if (!container)
            return false;

        return container->Contains(name);
    }

    template<typename R>
    ResourcePtr<R> ResourceManager::Resolve(const std::string &source)
    {
        auto loader = ResourceLoaderFactory::GetLoader<R>();
        if (!loader)
            return nullptr;

        auto metaContainer = GetContainer<ResourceMetadata>();
        if (loader->IsMetadataRequired() && metaContainer)
        {
            if (auto metadata = metaContainer->Find(source))
                return loader->Load(*metadata, ResolveContext(*metadata));
        }

        Uint8 *data;
        if (auto size = GetResourceData(source, &data))
        {
            ResourcePtr<R> resource;
            if (loader->IsMetadataRequired())
            {
                auto metadata = loader->LoadMetadata(data, size);
                if (!metadata)
                    return nullptr;

                if (metaContainer)
                {
                    auto cachedMeta = metaContainer->Add(source, std::move(metadata));
                    resource = loader->Load(*cachedMeta, ResolveContext(*cachedMeta));
                }
                else
                    resource = loader->Load(*metadata, ResolveContext(*metadata));

                // metadata can be deleted immediately
                delete[] data;
                data = nullptr;

                return resource;
            }
            else
            {
                resource = loader->Load(data, size);
                if (!loader->IsResourceStream())
                {
                    delete[] data;
                    data = nullptr;
                }

                std::function<void(R *)> deleter = [data, loader](auto ptr) {
                    delete ptr;
                    if (loader->IsResourceStream())
                        delete[] data;
                };

                return ResourcePtr<R>{resource.release(), deleter};
            }
        }

        return nullptr;
    }

    template<typename R>
    ResourceMetadata *ResourceManager::LoadMetadata(const std::string &source)
    {
        auto container = GetContainer<ResourceMetadata>();
        if (!container)
            return nullptr;

        if (auto metadata = container->Find(source))
            return dynamic_cast<ResourceMetadata*>(metadata);

        auto loader = ResourceLoaderFactory::GetLoader<R>();
        if (!loader || !loader->IsMetadataRequired())
            return nullptr;

        Uint8 *data;
        if (auto size = GetResourceData(source, &data))
        {
            auto metadata = loader->LoadMetadata(data, size);

            delete[] data;
            data = nullptr;

            return dynamic_cast<ResourceMetadata*>(container->Add(source, std::move(metadata)));
        }

        return nullptr;
    }

    template<typename R>
    R* ResourceManager::Load(const std::string &source)
    {
        auto container = GetContainer<R>();
        if (!container)
            return nullptr;

        auto resolver = [this, source] { return Resolve<R>(source); };
        return container->Add(source, resolver, true);
    }

    template<typename R>
    R *ResourceManager::Load(const ResourceMetadata& metadata)
    {
        return Load<R>(metadata.Name, metadata);
    }

    template<typename R>
    R *ResourceManager::Load(const std::string &name, const ResourceMetadata &metadata)
    {
        auto container = GetContainer<R>();
        auto loader    = ResourceLoaderFactory::GetLoader<R>();
        if (!container || !loader)
            return nullptr;

        auto resolver = [this, loader, &metadata] { return loader->Load(metadata, ResolveContext(metadata)); };
        return container->Add(name, resolver, true);
    }
}